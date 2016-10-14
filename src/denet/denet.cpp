/*  This file is part of denet. 
 *  denet is a tool to study Bovespa-listed companies. 
 *
 *  Copyright (c) 2016 - Potuz Vader potuz@potuz.net
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
#include <memory>
#include <string>
#include <iostream>
#include "dfp_database.h"
#include "dfp_utils.h"
#include "dfp_cvm_file.h"
#include <locale>
#include "ezOptionParser/ezOptionParser.hpp"
#include "dfp_exception.h"
#ifdef _WIN32
nada          
#elif defined (__linux)
#include "termios.h" // To hide passwords on the terminal
#include "unistd.h"
#endif

using namespace ez;
using namespace Dfp;
void Usage(ezOptionParser& opt) {
  std::string usage;
  opt.getUsage(usage);
  std::cout << usage;
};
int main (int argc, const char * argv[] ) {
  std::locale::global ( std::locale(""));
  std::cout.imbue (std::locale(""));
  ezOptionParser opt;
  opt.overview = "Denet - Análise fundamentalista de companhias de capital "
    "aberto.";
  opt.syntax = "denet [(-i | --importe) arquivo] [ (-c|--cvm) cvm] "
    "[(-t|--ticker|--codigo) código ] [--individual || --consolidado]"
    "[ (-d|--data) exercício] [ (--ind | --indicador) (Indicador | número) ]"
    "[-r |--reporte] [-res | --resumo] [--anual] [ (-s | --senha) senha]" 
    "[--servidor nome ]";
  opt.example = "denet --importe 00538020160630301.zip\n"
    "denet --cvm 11592 --indicador MB --data 2016-06-30\n"
    "denet --senha segredo --codigo UNIP5 --indicador 3.01 --anual \n"
    "denet --cvm 11592 --reporte\n"
    "denet --senha segredo --servidor www.potuz.net"
    "--codigo UNIP5 --resumo\n\n";
  opt.footer = "denet 1.0 Copyright (C) 2016 Potuz potuz@potuz.net\n"
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
  opt.add("", 0,-1,',',"Arquivo(s) a ser(em) importado(s).", "-i", 
      "--importe");
  opt.add ("", 0, 1, 0, "Data do exercício.", "-d", "--data");
  opt.add ("", 0, 1, 0, "Senha do usuário MySQL.", "-s", "--senha");
  opt.add ("localhost", 0, 1, 0, "Servidor MySQL.", "--servidor");
  opt.add ("", 0, 1, 0, "Código cvm da empresa.", "-c", "--cvm");
  opt.add ("", 0, 1, 0, "Código de negociação na BMF&Bovespa", "-t",
      "--ticker", "--codigo"); 
  opt.add("", 0, 0, 0, "Resultado acumulado dos últimos 12 meses", "-a",
      "--anual");
  opt.add ("", 0, 0, 0, "Considerar resultado Individual.", "--individual");
  opt.add ("", 0, 0, 0, "Considerar resultado Consolidado (por defeito).", 
      "--consolidado");
  opt.add("", 0, 1, 0, "Indicador ou número da conta no demontrativo "
      "financeiro da empresa.", "--ind", "--indicador");
  opt.add("", 0, 1, 0, "Reporte do resultado.", "-r", "--reporte");
  opt.add("", 0, 0, 0, "Resumo da empresa.", "-res", "--resumo");
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
  try { 
    conn = std::make_shared<Dfp::Database> (host, "denet", passwd); 
  } catch ( sql::SQLException &e ) {
    switch ( e.getErrorCode() ) {
      case 1045:  // bad password
        {
          std::cout << "A senha ingressada não confere. Tente novamente.\n\n";
          std::exit(1);
          break;}
      case 1044: { // user does not exist
                   std::cout << "\n\n############\n";
                   std::cout << "Essa parece ser a primeira vez que você está ";
                   std::cout << "utilizando denet neste computador.\n\n";
                   std::cout << "A continuação o programa criará um usuário novo para";
                   std::cout << " administrar a base de dados.\nVocê precisará de ter";
                   std::cout << " os dados do usuário administrador de MySQL no seu ";
                   std::cout << "computador. É possível indicar um servidor diferente ";
                   std::cout << "do que o computador local, sempre que tenha configurado";
                   std::cout << " o MySQL\n";
back_to_question: 
                   std::cout << "\nDeseja continuar? (s/n) ";
                   std::string E;
                   std::getline (std::cin, E);
                   if ( (E.compare("n") == 0) || (E.compare ("N") == 0) || E.empty() ) 
                     std::exit (1);
                   else if ( E.compare("S") && E.compare("s") ) goto back_to_question;

                   std::cout << "Usuário administrador de MySQL: ";
                   std::string rootusr;
                   getline (std::cin, rootusr);
                   std::string rootpasswd;
                   for (int i = 0; i < 3 ; i++) { 
                     std::cout << "Senha: ";
                     #ifdef _WIN32
http://stackoverflow.com/questions/6899025/hide-user-input-on-password-prompt  
                     #elif defined (__linux)
                     termios oldt;
                     tcgetattr(STDIN_FILENO, &oldt);
                     termios newt = oldt;
                     newt.c_lflag &= ~ECHO;
                     tcsetattr(STDIN_FILENO, TCSANOW, &newt);
                     getline (std::cin, rootpasswd);
                     std::cout << "\nReitere a senha: ";
                     std::string recheck;
                     getline (std::cin, recheck);
                     tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                     #endif
                     if ( recheck.compare(rootpasswd) ) 
                       std::cout << "As senhas inseridas não conferem. Tente novamente.\n";
                     else break;
                   };

                   std::cout << "\nServidor (localhost): ";
                   std::string host;
                   getline (std::cin, host);
                   if (host.empty()) host = "localhost";
                   std::cout << "\nAgora criaremos o novo usuário \"denet\". Para isso ";
                   std::cout << "precisamos uma nova senha que não precisa ser igual ";
                   std::cout << "á senha do administrador.\n";
                   std::string denetpwd;
                   for (int i = 0; i < 3 ; i++) { 
                     std::cout << "Digite a nova senha: ";
                     #ifdef _WIN32
                     nada;
                     #elif defined (__linux)
                     termios oldt;
                     tcgetattr(STDIN_FILENO, &oldt);
                     termios newt = oldt;
                     newt.c_lflag &= ~ECHO;
                     tcsetattr(STDIN_FILENO, TCSANOW, &newt);
                     getline (std::cin, denetpwd);
                     std::cout << "\nReitere a senha: ";
                     std::string recheck;
                     getline (std::cin, recheck);
                     tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                     #endif
                     if ( recheck.compare(denetpwd) ) 
                       std::cout << "As senhas inseridas não conferem. Tente novamente.\n";
                     else break;
                   };
                   std::cout << "Lembre de guardar essa senha";
                   std::cout << "para futura utilização.\n";
                   try { 
                     Dfp::setup_dbase ( rootusr, rootpasswd, host, denetpwd );
                   } catch ( sql::SQLException &e ) 
                   { 
                     switch ( e.getErrorCode() ) {
                       case 1045: 
                         std::cerr << "\n\nA senha do administrador MySQL não confere!\n";
                         std::cerr << "Tente novamente.\n";
                         std::exit(1);
                         break;
                       case 2005: 
                         std::cerr << "\n\nO servidor \"" << host << "\" não é um";
                         std::cerr << " servidor MySQL valido.\n";
                         std::exit(1);
                         break;
                     }
                     throw;
                   }
                   std::exit(1);
                   break;
                 }
      case 2005: 
                 std::cerr << "\n ERRO: O servidor \"" << host << "\" não é um";
                 std::cerr << " servidor MySQL valido.\n";
                 std::exit(1);
                 break;
      default: std::cout << "Error Code: " << e.getErrorCode() << "\n"; 
    }
    throw;
  }
  if (opt.isSet("-i")) {
    std::vector<std::string> files;
    opt.get("-i")->getStrings(files);
    for (unsigned int j=0; j < files.size(); ++j)
    {
      Dfp::CvmFile cvmfile ( files[j] );
      try { 
        cvmfile.import (*conn);
      } catch ( Dfp::Exception &e ) { 
        if ( e.getErrorCode()  == EXCEPTION_NO_INTERNET ) {
          std::cerr << "ERRO: Sem acesso a internet. "; 
          std::cerr << "Controle a sua conexão.\n";
          exit (1);
        }
        throw;
      }
    }
    return 0;
  };
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
  else opt.get("-c")->getInt (cvm);
  Dfp::FinancialInfoType type = Dfp::DFP_FINANCIAL_INFO_CONSOLIDATED;
  if  ( opt.isSet("--individual") ) {
    if ( opt.isSet ( "--consolidado" ) ) {
      std::cerr << "ERRO: só uma de \"--individual\" ou \"--consolidado\" ";
      std::cerr << "pode ser utilizado ao mesmo tempo.\n";
      Usage (opt);
      return 1;
    }
    type = Dfp::DFP_FINANCIAL_INFO_INDIVIDUAL;
  }
  if (opt.isSet ("--indicador")) { 
    if ( !opt.isSet ("-c") && !opt.isSet("-t") ) {
      std::cerr << "ERRO: para utilizar --ind pelo menos um de \"-c\" ou ";
      std::cerr << "\"-t\" é necessário. \n"; 
      Usage(opt);
      return 1;
    }
    std::string indstr;
    opt.get("--indicador")->getString (indstr);
    bool anual = false;
    if  (opt.isSet("--anual") ) anual = true;
    const std::vector<std::pair<std::string, Dfp::Indicator>> indicators = {
      { "MV", DFP_INDICATOR_MV }, { "VM", DFP_INDICATOR_MV}, 
      { "PE", DFP_INDICATOR_PE}, { "P/L", DFP_INDICATOR_PE}, 
      { "EBIT", DFP_INDICATOR_EBIT}, {"P/EBIT", DFP_INDICATOR_PEBIT}, 
      { "PSR", DFP_INDICATOR_PSR}, { "P/ATIVOS", DFP_INDICATOR_PASSET}, 
      { "CAPGIRO", DFP_INDICATOR_WORKCAP}, 
      { "P/CAPGIRO", DFP_INDICATOR_PWORKCAP }, 
      { "ATIVOSLIQNET", DFP_INDICATOR_NETLIQASSET}, 
      { "P/ATIVOSLIQNET", DFP_INDICATOR_PNETLIQASSET},
      { "DIVIDA", DFP_INDICATOR_LIAB}, { "DIVIDANETA", DFP_INDICATOR_NETLIAB},
      { "VE", DFP_INDICATOR_EV}, { "VE/EBIT", DFP_INDICATOR_EVEBIT}, 
      { "LPA", DFP_INDICATOR_EARNSHARE }, { "VPA", DFP_INDICATOR_VPA },
      { "MB", DFP_INDICATOR_MB }, { "MEBIT", DFP_INDICATOR_MEBIT },
      { "ML", DFP_INDICATOR_ML}, { "EBIT/ATIVOS", DFP_INDICATOR_EBITASSETS},
      { "ROE", DFP_INDICATOR_ROE}, { "ROIC", DFP_INDICATOR_ROIC},
      { "LIQCORR", DFP_INDICATOR_LIQCORR }, 
      { "DIV/PL", DFP_INDICATOR_DIVBRPL}, { "P/VP", DFP_INDICATOR_PVP}, 
      { "DY", DFP_INDICATOR_DY}, { "PL", DFP_INDICADOR_PL } };
    unsigned int i = 0;
    Indicator indenum;
    while ( i++ < indicators.size() ) 
      if ( indstr.compare ( indicators[i-1].first ) == 0 ) {
        indenum = indicators[i-1].second;
        break;
      };
    if ( i > indicators.size() ) indenum = DFP_INDICATOR_INVALID;
    if ( !opt.isSet ("-d") ) {
      if ( indenum != DFP_INDICATOR_INVALID ) { 
        float result;
        try {
          result = conn->get_indicator (cvm, indenum, anual, type);
        } catch ( Dfp::Exception &e ) {
          switch ( e.getErrorCode () ) { 
            case EXCEPTION_NO_ACCT: 
              std::cerr << "\n\nERRO: Aparentemente não encontramos a "
                "conta solicitada.";
              std::cerr << "\nVerifique se a conta se encontra na base de ";
              std::cerr << "dados. \nUma opção é considerar --individual ou";
              std::cerr << " --consolidado.\n\n";
              std::cerr << e.what() << std::endl;
              std::exit (1);
              break;
            default: throw;
          }
        }
        std::cout << "Valor: ";
        std::cout << result << std::endl;
        return 0;
      } 
      else {
        std::cout << "Valor: ";
        std::cout << conn->get_indicator (cvm, indstr, anual, type);
        std::cout << std::endl;
        return 0;
      }
    }
    else { 
      std::string datearg;
      opt.get("-d")->getString( datearg );

      //TODO: use iomanip get_time when available c++17
      std::tm tm;
      std::stringstream date_str ( datearg ); 
      date_str >> tm.tm_year; 
      date_str.ignore();
      date_str >> tm.tm_mon;
      date_str.ignore();
      date_str >> tm.tm_mday;
      tm.tm_year -= 1900;
      tm.tm_mon--;
      tm.tm_hour = 0;
      tm.tm_min = 0;
      tm.tm_sec = 0;
      if ( indenum != DFP_INDICATOR_INVALID ) { 
        std::cout << "Valor: ";
        std::cout << conn->get_indicator (cvm, indenum, tm, anual, type);
        std::cout << std::endl;
        return 0; 
      } 
      else {
        std::cout << "Valor: ";
        std::cout << conn->get_indicator (cvm, indstr, tm,  anual, type);
        std::cout << std::endl;
        return 0;
      }
    }
  }
  std::cerr << "Erro: pelo menos um de [-i, -r, -res, --ind] ";
  std::cerr << "é necessário\n\n"; 
  Usage(opt);
  return 1;

}

