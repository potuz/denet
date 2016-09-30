/*  This file is part of denet. 
 *  denet is a tool to study Bovespa-listed companies. 
 *
 *  Copyleft (c) 2016 - Potuz Vader potuz@potuz.net
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
#include "dfp_cvm_connection.h"
#include <string>
#include <curl/curl.h>
#include <regex>
#include "dfp_utils.h"

namespace {
  size_t write_cb(void *contents, size_t size, size_t nmemb, std::string *s) {
    size_t newLength = size*nmemb;
    size_t oldLength = s->size();
    try
    {
      s->resize(oldLength + newLength);
    }
    catch(std::bad_alloc &e)
    {
      Dfp::debug_log ( "Error: sem memoria suficiente\n" );
      return 0;
    }

    std::copy((char*)contents,(char*)contents+newLength,s->begin()+oldLength);
    return size*nmemb;
  }
}
namespace Dfp {
  namespace Cvm {
    CvmUrl last_available_for_download ( int cvm, 
        Dfp::CvmFileType filetype ) { 
      return available_for_download( cvm, filetype, 1).front();
    }

    std::vector<CvmUrl> available_for_download ( int cvm, 
        Dfp::CvmFileType filetype, int links) {
      std::vector<CvmUrl> result;
      CURL *curl;
      CURLcode res;
      curl_global_init(CURL_GLOBAL_ALL);
      curl = curl_easy_init();
      std::string s;
      if(curl)
      {
        std::string url_str = "http://siteempresas.bovespa.com.br/consbov/"
            "ExibeTodosDocumentosCVM.asp?CCVM=" + std::to_string( cvm ) + 
            "&TipoDoc=C&QtLinks=" + std::to_string (links);
        curl_easy_setopt( curl, CURLOPT_URL, url_str.c_str() );
        std::string hdncategory;
        switch (filetype) { 
          case DFP_CVM_FILE_DFP: hdncategory = "IDI2";
                                 break;
          case DFP_CVM_FILE_ITR: hdncategory = "IDI1";
                                 break;
          case DFP_CVM_FILE_FRE: hdncategory = "FR";
                                 break;
          default: return result;
        }
        std::string postfields = "hdnCategoria="+ hdncategory + 
          "&hdnPagina=&FechaI=&FechaV=";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
          std::string error_str = "Erro tentando obter página da CVM: ";
          error_str.append ( curl_easy_strerror(res) );
          Dfp::debug_log ( error_str );
        }
        curl_easy_cleanup(curl);
      }
      std::istringstream mystream (s);
      std::string line;
      std::regex rgx (".* - Ativo[^]*", std::regex::ECMAScript);
      std::regex rgx2 (
          ".*fVisualizaArquivo_ENET.\'([0-9]+)\',\'DOWNLOAD.*");
      std::regex rgxdate ( 
          ".*([0-9][0-9]/[0-9][0-9]/[0-9]{4})[^]*");
      std::regex rgxversion ( ".*nowrap>([0-9]\\.[0-9]).[^]*");
      std::smatch match_file, match_date, match_version;
      while ( getline( mystream, line ) )
      { 
        if ( std::regex_match ( line.begin(), line.end(), rgx ) ) {
          while (getline (mystream, line ) ) {
            const std::string line1 (line);
            if ( std::regex_search (line1.begin(), line1.end(), 
                  match_file, rgx2 ) ) {
              while (getline (mystream, line ) ) {
                const std::string line2 (line);
                if (std::regex_search (line2.begin(), line2.end(),
                      match_date, rgxdate ) ) { 
                  while ( getline (mystream, line ) ) {
                    const std::string line3 (line);
                    if (std::regex_search ( line3.begin(), line3.end(),
                          match_version, rgxversion ) ){
                      CvmUrl entry;
                      entry.protocol = std::stoi ( match_file[1] );
                      entry.date_str = match_date[1];
                      entry.revision = std::stoi (match_version[1]);
                      result.push_back (entry);
                      break;
                    }
                  }
                  break;
                }
              }
              break;
            }
          }
        }
      }
      return result;
    }
  }

}
