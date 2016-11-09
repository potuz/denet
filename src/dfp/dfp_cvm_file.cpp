/*  This file is part of denet. 
 *  denet is a tool to study Bovespa-listed companies. 
 *
 *  Copyright (c) 2016  potuz@potuz.net
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "dfp_exception.h"
#include <archive.h>
#include <archive_entry.h>
#include <stdexcept>
#include <cstring> // strcmp 
#include <sstream> // std::stringstream
#include <cstdio>  // std::remove
#include <vector>
#include <algorithm> //std::find
#include "pugixml/pugixml.hpp"
#include "dfp_database.h" 
#include "dfp_company.h"
#include "dfp_utils.h"
#include <curl/curl.h>

namespace {
  enum DfpScale {
    DFP_SCALE_UNIT = 1,
    DFP_SCALE_THOUSAND 
  };
  const  std::vector<std::string> needed_files {"Documento.xml", 
    "ComposicaoCapitalSocialDemonstracaoFinanceiraNegocios.xml", 
    "InfoFinaDFin.xml" };
  const std::vector<std::string> share_number_acct { "1.89.01", "1.89.02",
    "1.89.03", "1.89.04", "1.89.05", "1.89.06" };
  std::vector<std::string> share_number_string {
    "QuantidadeAcaoOrdinariaCapitalIntegralizado",
      "QuantidadeAcaoPreferencialCapitalIntegralizado",
      "QuantidadeTotalAcaoCapitalIntegralizado",
      "QuantidadeAcaoOrdinariaTesouraria",
      "QuantidadeAcaoPreferencialTesouraria", 
      "QuantidadeTotalAcaoTesouraria" };
  inline std::istream & operator>>(std::istream & str, Dfp::CvmFileType & v) {
    unsigned int ltype = 0;
    if (str >> ltype)
      v = static_cast<Dfp::CvmFileType>(ltype);
    return str;
  }
  void copy_data(struct archive *ar, struct archive *aw) {
    int r;
    const void *buff;
    size_t size;
    int64_t offset;

    for (;;) {
      r = archive_read_data_block(ar, &buff, &size, &offset);
      if (r == ARCHIVE_EOF)
        return;
      if (r != ARCHIVE_OK)
        throw std::runtime_error ( 
            " copy_data(): r!= ARCHIVE_OK ");
      r = archive_write_data_block(aw, buff, size, offset);
      if (r != ARCHIVE_OK) throw std::runtime_error (
          archive_error_string(aw));
    }
  }

  size_t write_callback(char *ptr, size_t size, size_t nmemb, 
      int *userdata) {
    std::istringstream stream (ptr);
    std::string line;
    //TODO: Handle better the not-found case, perhaps get last known quote
    //In any case throw an exception to let the user know we have an invalid
    //price. 
    std::getline (stream, line, ',');
    if ( line != "Date" ) { 
      *userdata = 0;
      return size*nmemb;
    }
    std::getline (stream, line);
    for (int i = 0; i < 5; i++ ) std::getline (stream, line, ',');
    double result = std::stod (line) * 100;
    *userdata =  ( int ) result;
    return size*nmemb;
  }
}
void Dfp::CvmFile::unzip ( const std::string& path ) {
  struct archive *a, *a_b;
  struct archive *ext ;
  struct archive_entry *entry;
  int r;
  std::string entry_pathname;
  std::string entry_full_pathname, entry_full_pathname_b;

  a = archive_read_new();
  ext = archive_write_disk_new();
  archive_read_support_format_zip(a);

  std::string extension;
  switch ( Dfp::CvmFile::type ) {
    case DFP_CVM_FILE_ITR: extension = "itr"; break;
    case DFP_CVM_FILE_DFP: extension = "dfp"; break;
    default: return;
  }
  if ((r = archive_read_open_filename(a, filename.c_str(), 10240)))
    throw std::runtime_error ( archive_error_string(a) );

  debug_log ("CvmFile::unzip opened " + filename + " for reading.\n");
  for (;;) {
    r = archive_read_next_header(a, &entry);
    if (r == ARCHIVE_EOF)
      break;
    if (r != ARCHIVE_OK)
      throw std::runtime_error ( archive_error_string(a) );
    entry_pathname = archive_entry_pathname (entry); 
    if ( !extension.empty() && 
        strcmp (&entry_pathname[19], extension.c_str() ) == 0) {
      entry_full_pathname = path + entry_pathname;
      archive_entry_set_pathname (entry, entry_full_pathname.c_str() );
      r = archive_write_header(ext, entry);
      if (r != ARCHIVE_OK)
        throw std::runtime_error ( archive_error_string(ext));
      else {
        copy_data(a, ext);
        r = archive_write_finish_entry(ext);
        if (r != ARCHIVE_OK)
          throw std::runtime_error ( archive_error_string(ext) );
      }
      a_b = archive_read_new();
      archive_read_support_format_zip(a_b);
      debug_log (
          "CvmFile::unzip about to open " + entry_full_pathname + ".\n");
      if ((r = archive_read_open_filename(a_b, 
              entry_full_pathname.c_str(), 10240)))
        throw std::runtime_error ( archive_error_string(a_b) );
      for (;;) {
        debug_log ( "CvmFile::unzip inside second for loop\n");
        r = archive_read_next_header(a_b, &entry);
        if (r == ARCHIVE_EOF)
          break;
        if (r != ARCHIVE_OK)
          throw std::runtime_error ( archive_error_string(a_b) );
        entry_pathname = archive_entry_pathname (entry); 
        debug_log ( "CvmFile::unzip got pathname: "+entry_pathname+"\n");
        if ( std::find ( std::begin (needed_files), std::end(needed_files), 
              entry_pathname) != std::end(needed_files)) {
          entry_full_pathname_b = path + entry_pathname;
          archive_entry_set_pathname (entry, entry_full_pathname_b.c_str() );
          r = archive_write_header(ext, entry);
          if (r != ARCHIVE_OK)
            throw std::runtime_error ( archive_error_string(ext));
          else {
            copy_data(a_b, ext);
            r = archive_write_finish_entry(ext);
            if (r != ARCHIVE_OK)
              throw std::runtime_error ( archive_error_string(ext) );
          }
          debug_log ( 
              "CvmFile::unzip inflated " + entry_full_pathname_b + ".\n"); 
        }
      }
      archive_read_close(a_b);
      archive_read_free(a_b);
      debug_log ( 
          "CvmFile::unzip about to remove " + entry_full_pathname + ".\n");
      std::remove ( entry_full_pathname.c_str() );
    }
  }
  archive_read_close(a);
  archive_read_free(a);

  archive_write_close(ext);
  archive_write_free(ext);

}
Dfp::CvmFile::CvmFile (const std::string& name) {
  const std::string invalid_file = "Not a valid CVM file"; 
  #ifdef _WIN32  
  std::string base_filename = name.substr(name.find_last_of("/\\") + 1);
  #elif defined (__linux)
  std::string base_filename = name.substr(name.find_last_of("/") + 1);
  #endif
  if ( base_filename.length() != 21 ) 
    throw std::invalid_argument ( name + ": " + invalid_file  );
  if (base_filename.compare ( 18, 3 , "zip" )) 
    throw std::invalid_argument ( name + ": " +  invalid_file );

  //TODO: use iomanip get_time when available c++17
  std::string myname ( base_filename );
  myname.insert (17, 1, ' ');
  myname.insert (15, 1, ' ');
  myname.insert (14, 1, ' ');
  myname.insert (12, 1, ' ');
  myname.insert (10, 1, ' ');
  myname.insert (6, 1, ' ');

  std::stringstream mystream ( myname );
  mystream >> cvm >> exercise.tm_year >> exercise.tm_mon >> 
    exercise.tm_mday >> type >> revision;  


  exercise.tm_hour = 0;
  exercise.tm_min = 0;
  exercise.tm_sec = 0;
  exercise.tm_mon--;
  exercise.tm_year -= 1900;

  filename = name;
}
void Dfp::CvmFile::import (const Dfp::Database &conn) { 
  Dfp::DatabaseAccount acct; 

  if ( type != DFP_CVM_FILE_DFP && type != DFP_CVM_FILE_ITR ) return;
  Dfp::Company company = conn.get_company_from_cvm (cvm); 
  int last_revision =  company.last_imported_revision (exercise);
  if ( revision <= last_revision ) return;
  if ( last_revision > 0 ) {
    company.delete_exercise ( exercise );
  }
  else company.create_table ();

  //TODO Do this with <iomanip> when available 
  std::string date_str;
  date_str.resize(11);
  strftime(&date_str[0], date_str.size(), "%Y-%m-%d", &exercise);

  std::vector<Dfp::Ticker> tickers = company.get_tickers();

  acct.date = date_str;
  acct.comments = "";
  acct.balance_type = DFP_BALANCE_IF;
  acct.financial_info_type = DFP_FINANCIAL_INFO_CONSOLIDATED;

  CURL *curl;
  CURLcode res;
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if(curl) {
    for ( std::vector<Dfp::Ticker>::iterator it = tickers.begin(); 
        it != tickers.end(); ++it ) {
      acct.number = "1.90.";
      acct.name = "Preço 100x";
      acct.name.append ( it->code );
      int i = static_cast<int> (it->type );
      if  ( i < 10 ) acct.number.append ( "0");
      acct.number.append ( std::to_string (i) ); 
      std::string url =  "http://ichart.yahoo.com/table.csv?s="
        + it->code + ".SA&d=" + std::to_string ( exercise.tm_mon + 1 )+
        "&e=" + std::to_string ( exercise.tm_mday ) + "&f=" + 
        std::to_string ( exercise.tm_year + 1900 ) + "&a=" + 
        std::to_string ( exercise.tm_mon+1 ) + "&b=" + 
        std::to_string ( exercise.tm_mday - 7 ) + "&c=" + 
        std::to_string ( exercise.tm_year + 1900) ;
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
      curl_easy_setopt(curl,CURLOPT_NOSIGNAL, 1);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &acct.value);
      res = curl_easy_perform(curl);
      if(res != CURLE_OK) 
        throw Dfp::Exception ( curl_easy_strerror(res), EXCEPTION_NO_INTERNET);
      conn.import_account ( cvm, acct ); 
    } 
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();

  //TODO: use <filesystem> when available in GCC5. 
  std::string temporary_path;
  #ifdef _WIN32
  nada;
  #elif defined (__linux)
  const char* tmpdir = getenv ("TMPDIR");
  if ( tmpdir ) 
    std::string temporary_path ( tmpdir );
  else 
    temporary_path = "/tmp";
  temporary_path.append( "/denet-XXXXXX");
  if (!mkdtemp( &temporary_path[0]) ) throw std::runtime_error ("Cannot create "
      "temporary directory");
  temporary_path.append ("/");
  #endif
  debug_log ("CvmFile::import(): About to unzip " + filename + " to " 
      + temporary_path + ".\n");
  unzip ( temporary_path );
  debug_log ("CvmFile::import() unzipped file, parsing xml...\n");

  pugi::xml_document xmldoc;
  std::string xmlfilename = temporary_path + needed_files[0]; // Documento.xml
  if (!xmldoc.load_file ( xmlfilename.c_str() )) throw std::runtime_error (
      needed_files[0] + ": Cannot open XML" );
  int money_scale = std::stoi ( 
      xmldoc.child("Documento").child_value("CodigoEscalaMoeda") );
  int quant_scale = std::stoi (
      xmldoc.child("Documento").child_value("CodigoEscalaQuantidade") );
  money_scale = 1999 - money_scale * 999; 
  quant_scale = 1999 - quant_scale * 999;
  debug_log("CvmFile::import() parsed Documento.xml: " + 
      std::to_string(money_scale) + "\n About to parse " + 
      "ComposicaoCapital...\n" );
  xmlfilename = temporary_path + needed_files[1]; // Composicao...
  if (!xmldoc.load_file ( xmlfilename.c_str() )) throw std::runtime_error (
      needed_files[1] + ": Cannot open XML" );
  for ( int i = 0; i < 6; i++ ) {
    acct.value = std::stoll (
        xmldoc.child("ArrayOfComposicaoCapitalSocialDemonstracaoFinanceira").\
        child("ComposicaoCapitalSocialDemonstracaoFinanceira").\
        child_value(share_number_string[i].c_str() ) );
    acct.value  = (int) acct.value / quant_scale;
    acct.balance_type = DFP_BALANCE_IF;
    acct.financial_info_type = DFP_FINANCIAL_INFO_CONSOLIDATED;
    acct.number = share_number_acct[i];
    acct.name = share_number_string[i];
    acct.comments = "";
    acct.date = date_str;
    debug_log("CvmFile::import() in shares loop. Sending account " + 
        acct.name + " : " + std::to_string(acct.value));
    conn.import_account (cvm, acct);
  }
  xmlfilename = temporary_path + needed_files[2]; // InfoFinaDFin
  if (!xmldoc.load_file ( xmlfilename.c_str() )) throw std::runtime_error (
      needed_files[2] + ": Cannot open XML" );
  for ( pugi::xml_node info_node = xmldoc.child("ArrayOfInfoFinaDFin").\
      child("InfoFinaDFin"); info_node; info_node = info_node.next_sibling(
        "InfoFinaDFin")) {
    pugi::xml_node versionacct = info_node.child("PlanoConta").\
                                 child("VersaoPlanoConta");
    acct.balance_type = static_cast<Dfp::BalanceType> ( std::stoi ( 
          versionacct.child_value ( "CodigoTipoDemonstracaoFinanceira" ) ) );
    acct.financial_info_type = static_cast<Dfp::FinancialInfoType>( std::stoi (
          versionacct.child_value ( "CodigoTipoInformacaoFinanceira" )) );
    acct.number = info_node.child("PlanoConta").child_value("NumeroConta");
    acct.name = info_node.child_value("DescricaoConta1");
    acct.date = date_str;
    switch ( acct.balance_type ) {
      case DFP_BALANCE_DMPL: {
                               int periodo = std::stoi ( info_node.\
                                   child("PeriodoDemonstracaoFinanceira").\
                                   child_value("NumeroIdentificacaoPeriodo") );
                               if ( ( type == DFP_CVM_FILE_DFP && periodo != 1) || 
                                   ( type == DFP_CVM_FILE_ITR && periodo != 4) ) break;
                               acct.comments = "Capital social integralizado";
                               acct.value = std::stoi ( 
                                   info_node.child_value ( "ValorConta1" ) );
                               conn.import_account ( cvm, acct );
                               acct.comments = "Reservas de capital";  
                               acct.value = std::stoi ( 
                                   info_node.child_value ( "ValorConta2" ) );
                               acct.value = (int) acct.value / money_scale;
                               conn.import_account ( cvm, acct );
                               acct.comments = "Reservas de lucro";  
                               acct.value = std::stoi ( 
                                   info_node.child_value ( "ValorConta3" ) );
                               acct.value = (int) acct.value / money_scale;
                               conn.import_account ( cvm, acct );
                               acct.comments = "Lucros/Prejuízos acumulados";  
                               acct.value = std::stoi ( 
                                   info_node.child_value ( "ValorConta4" ) );
                               acct.value = (int) acct.value / money_scale;
                               conn.import_account ( cvm, acct );
                               acct.comments = "Outros resultados abrangentes";  
                               acct.value = std::stoi ( 
                                   info_node.child_value ( "ValorConta5" ) );
                               acct.value = (int) acct.value / money_scale;
                               conn.import_account ( cvm, acct );
                               acct.comments = "Patrimônio Líquido";  
                               acct.value = std::stoi ( 
                                   info_node.child_value ( "ValorConta6" ) );
                               acct.value = (int) acct.value / money_scale;
                               conn.import_account ( cvm, acct );
                               break; }
      default:
                             acct.comments = "";
                             switch ( type ) {
                               case DFP_CVM_FILE_DFP:
                                 acct.value = std::stoi ( info_node.child_value ("ValorConta1") );
                                 acct.value = (int) acct.value / money_scale;
                                 conn.import_account ( cvm, acct );
                                 break;
                               case DFP_CVM_FILE_ITR:
                                 switch ( acct.balance_type ) {
                                   case DFP_BALANCE_BPA:
                                   case DFP_BALANCE_BPP:
                                     acct.value = std::stoi ( 
                                         info_node.child_value ("ValorConta2") );
                                     acct.value = (int) acct.value / money_scale;
                                     break;
                                   case DFP_BALANCE_DFC_MD:
                                   case DFP_BALANCE_DFC_MI:
                                   case DFP_BALANCE_DVA:
                                     acct.value = std::stoi ( 
                                         info_node.child_value ("ValorConta4") );
                                     acct.value = (int) acct.value / money_scale;
                                     break;
                                   default:
                                     if ( exercise.tm_mon == 2 ) {
                                       acct.value = std::stoi ( 
                                           info_node.child_value ("ValorConta4") );
                                       acct.value = (int) acct.value / money_scale; 
                                     }
                                     else {
                                       acct.value = std::stoi ( 
                                           info_node.child_value ("ValorConta2") );
                                       acct.value = (int) acct.value / money_scale; 
                                     }
                                     break;
                                 }
                                 conn.import_account ( cvm, acct );
                                 break;
                               default:
                                 break;
                             }
    }
  }
  std::vector<std::string>::const_iterator r; 
  for (r = needed_files.begin(); r != needed_files.end(); ++r ) {
    //    r->insert (0, temporary_path); 
    std::string removable = temporary_path + *r;
    debug_log ( "CvmFile::import about to remove " + removable + "\n");
    std::remove ( removable.c_str() );
    debug_log ( "CvmFile::removed " + removable + "\n");
  }
  std::remove ( temporary_path.c_str() );
  company.add_revision ( revision, exercise );
}

