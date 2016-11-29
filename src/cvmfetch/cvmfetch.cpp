/*  This file is part of denet. 
 *  denet is a tool to study Bovespa-listed companies. 
 *
 *  Copyright (c) 2016 - Potuz potuz@potuz.net
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
#include "dfp_cvm_connection.h"
#include "ezOptionParser/ezOptionParser.hpp"
#include "dfp_database.h"
#include "dfp_exception.h"
#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#endif
using namespace ez;
using namespace Dfp;
namespace {
  void Usage(ezOptionParser& opt) {
    std::string usage;
    opt.getUsage(usage);
    std::cout << usage;
  };
  void process_available_files ( const std::shared_ptr<Dfp::Database> conn,
      int cvm, Dfp::CvmFileType filetype, bool download, std::string path = "" ) 
  {
    std::vector<CvmUrl> cvmurl;
    try {
      cvmurl = conn->available_for_download ( cvm, filetype);
    } catch ( std::invalid_argument &e ) { 
      std::cerr << "ERRO: tipo de arquivo invalido.\n";
      std::exit (1);
    } catch ( Dfp::Exception &e ) {
      if ( e.getErrorCode() == EXCEPTION_NO_INTERNET ) {
        std::cerr << "ERRO: Sem acesso a internet. "; 
        std::cerr << "Controle a sua conexão.\n";
        exit (1);
      }
      throw;
    }
    if ( !download ) { 
      std::cout << "Os seguintes arquivos ";
      switch ( filetype ) {
        case DFP_CVM_FILE_DFP: std::cout << "DFP";
                               break;
        case DFP_CVM_FILE_ITR: std::cout << "ITR";
                               break;
        case DFP_CVM_FILE_FRE: std::cout << "FRE";
                               break;
        default: break;
      }
      std::cout << " estão disponíveis para download na CVM:\n";
    }
    int j=cvmurl.size();
    int k=1;
    for ( auto it = cvmurl.begin(); it != cvmurl.end(); ++k, ++it) {
      if ( !download ) { 
        std::cout << "Data: "<<  it->date_str << " - ";
        std::cout << "Version: " << it->revision  << " - ";
        std::cout << "Protocolo: " << it->protocol << "\n";
      }
      else {
        std::cout << "Descargando arquivo " << k << " de " << j << ".\n";
        std::string filename = Dfp::Cvm::download ( 
            it->protocol, path );
        std::cout << "Descargado arquivo " << filename << " \n";
      }
    }
  }
}
int main (int argc, const char * argv[] ) {
  ezOptionParser opt;
  opt.overview = "cvmfetch - Download automático de arquivos da CVM.";
  opt.syntax = "cvmfetch [ (-h | --help | --ajuda ) "
    "(-l | --lista [dfp|itr|fre] ) [ (-c|--cvm) cvm] "
    "[ (-t | --ticker | --codigo) código ] (-s | --senha) senha]" 
    "[--servidor nome ] [ ( -d | --descarga ) protocolo ]"
    "[-D | --descarga-todos] ([ -i | --data-inicial ] data)" ;
  opt.example = "cvmfetch --list dfp 5380 -s senha\n"
    "cvmfetch -l EALT4 \n"
    "cvmfetch -d 05942\n"
    "cvmfetch -D -i 2014-04-29 -s meusegredo\n";
  opt.footer = "cvmfetch 1.0 Copyright (C) 2016 Potuz potuz@potuz.net\n"
    "This program is free and without warranty\n";
  opt.add(
      "", // Default.
      0, // Required?
      0, // Number of args expected.
      0, // Delimiter if expecting multiple args.
      "Modo de utilização.", // Help description.
      "-h",     // Flag token. 
      "--help", // Flag token.
      "--ajuda" // Flag token.
      );
  opt.add( "", 0, 1, 0, 
      "Lista os arquivos disponíveis na CVM para download. "
      "Aceita um parâmetro opcional com possíveis valores \"dfp\", "
      "\"itr\", \"fre\" ou \"todos\".", "-l", "--lista");
  opt.add ("", 0, 1, 0, "Código cvm da empresa.", "-c", "--cvm");
  opt.add ("", 0, 1, 0, "Código de negociação na BMF&Bovespa", "-t",
      "--ticker", "--codigo"); 
  opt.add ("", 0, 1, 0, "Senha do usuário MySQL.", "-s", "--senha");
  opt.add ("localhost", 0, 1, 0, "Servidor MySQL.", "--servidor");
  opt.add ("", 0, 1, 0, "Descarga um arquivo da CVM. O parâmetro"
      " é o número de protocolo que pode ser obtido com o comando "
      " cvmfetch -l.", "-d", "--descarga");
  opt.add ("", 0, 0, 0, "Descarrega todos os arquivos disponíveis para "
      "uma companhia. Este comando pode requerer de muito tempo dependendo "
      "da sua conexão de internet.", "-D", "--descarga-todos");
  opt.add ("", 0, 1, 0, "Data inicial para comando \"-D\". Só aqueles arquivos"
      " com exercício encerrado depois desta data serão descargados.", "-i", 
      "--data-inicial");
  opt.parse(argc, argv);
  std::vector<std::string> badOptions;
  if(!opt.gotExpected(badOptions)) {
    for(unsigned int i=0; i < badOptions.size(); ++i)
    {
      std::cerr << "ERRO: argumentos não esperados para ";
      std::cerr << badOptions[i] << ".\n\n";
    }
    Usage(opt);
    return 1;
  };
  std::vector<std::string> badArgs;
  if(!opt.gotValid(badOptions, badArgs)) {
    for(unsigned int i=0; i < badOptions.size(); ++i) {
      std::cerr << "ERRO: argumento invalido \"" << badArgs[i] ;
      std::cerr << "\" para opção " << badOptions[i] << ".\n\n";
    }
    //Usage(opt);
    return 1;
  };
  if (opt.isSet ("-h")) { 
    Usage(opt);
    return 1;
  };
  std::string passwd;
  if (opt.isSet("-s")) opt.get("-s")->getString (passwd);
  std::string host;
  opt.get("--servidor")->getString(host);
  std::shared_ptr<Dfp::Database> conn;
  if ( !opt.isSet ("-d") ) { 
    try { 
      conn = std::make_shared<Dfp::Database> (host, "denet", passwd); 
    } catch ( sql::SQLException &e ) {
      switch ( e.getErrorCode() ) {
        case 1045:  // bad password
          {
            std::cout << "A senha ingressada não confere. Tente novamente";
            std::cout << " especificando a senha correta com \"-s\".\n\n";
            std::exit(1);
            break;}
        case 2005: 
          std::cerr << "\n ERRO: O servidor \"" << host << "\" não é um";
          std::cerr << " servidor MySQL valido.\n";
          std::exit(1);
          break;
        default: exit(1);
      }
    }
  }
  if (opt.isSet ("-d") ) { 
    int protocol; 
    opt.get( "-d" )->getInt( protocol );
    //TODO: use <filesystem> when available in GCC5. 
    std::string temporary_path;
    #ifdef _WIN32
	TCHAR lpTempPathBuffer[100];
	DWORD dwRetVal = 0;
	dwRetVal = GetTempPath(100, lpTempPathBuffer);
	temporary_path.assign(lpTempPathBuffer);
    #elif defined (__linux)
    const char* tmpdir = getenv ("TMPDIR");
    if ( tmpdir ) 
      std::string temporary_path ( tmpdir );
    else 
      temporary_path = "/tmp/";
    #endif
    std::string filename = Dfp::Cvm::download ( protocol, temporary_path );
    std::cout << "Downloaded " << filename << "\n";
    return 0;
  }

  int cvm;
  std::string ticker;
  if (opt.isSet ("-t")) { 
    if ( opt.isSet("-c") ) { 
      std::cerr << "ERRO: só uma de \"-c\" ou \"-t\" pode ser utilizada ";
      std::cerr << "ao mesmo tempo.\n";
      Usage(opt);
      return 1;
    } 
    opt.get("-t")->getString (ticker);
    cvm = conn->get_cvm_from_ticker_str ( ticker );
  }
  else { 
    if (!opt.isSet("-c") ) { 
      std::cerr << "ERRO: pelo menos uma de \"-c\" ou \"-t\" "
        "deve ser utilizada.\n";
      Usage(opt);
      return 1;
    }
    opt.get("-c")->getInt (cvm);
  };

  std::string filetype_str;
  if (opt.isSet ("-l") ) { 
    opt.get("-l")->getString ( filetype_str );
    CvmFileType filetype = DFP_CVM_FILE_FALSE;
    if ( filetype_str.compare ( "dfp" ) == 0 || 
        filetype_str.compare ( "DFP" ) == 0) filetype = DFP_CVM_FILE_DFP;
    if ( filetype_str.compare ( "itr" ) == 0 || 
        filetype_str.compare ( "ITR" ) == 0) filetype = DFP_CVM_FILE_ITR;
    if ( filetype_str.compare ( "fre" ) == 0 || 
        filetype_str.compare ( "FRE" ) == 0) filetype = DFP_CVM_FILE_FRE;
    if ( filetype_str.compare ( "todos" ) != 0  ) 
      process_available_files ( conn, cvm, filetype, false );
    else {
      process_available_files ( conn, cvm, DFP_CVM_FILE_ITR, false );
      process_available_files ( conn, cvm, DFP_CVM_FILE_DFP, false );
      process_available_files ( conn, cvm, DFP_CVM_FILE_FRE, false );
    }
    return 0;
  }

  if (opt.isSet ("-D") ) {
    //TODO: use <filesystem> when available in GCC5. 
    std::string temporary_path;
    #ifdef _WIN32
	TCHAR lpTempPathBuffer[100];
	DWORD dwRetVal = 0;
	dwRetVal = GetTempPath(100, lpTempPathBuffer);
	temporary_path.assign(lpTempPathBuffer);
    #elif defined (__linux)
    const char* tmpdir = getenv ("TMPDIR");
    if ( tmpdir ) 
      std::string temporary_path ( tmpdir );
    else 
      temporary_path = "/tmp/";
    #endif
    process_available_files ( conn, cvm, DFP_CVM_FILE_ITR, 
        true, temporary_path );
    process_available_files ( conn, cvm, DFP_CVM_FILE_DFP, 
        true, temporary_path );
    process_available_files ( conn, cvm, DFP_CVM_FILE_FRE, 
        true, temporary_path );
    return 0;
  }

  std::cerr << "ERRO: pelo menos uma de \"-D\", \"-d\" ou \"-l\"  "
    " deve ser utilizada.\n";
  return 1;
}




