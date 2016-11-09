/*  This file is part of denet. 
 *  denet is a tool to study Bovespa-listed companies. 
 *
 *  Copyright (c) 2016 Potuz potuz@potuz.net
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
#include "dfp_database.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "dfp_company.h"
#include "mysql_driver.h"
#include "cppconn/statement.h"	// sql::Statement
#include "cppconn/prepared_statement.h"
#include "dfp_utils.h" // debug_log
#include "dfp_exception.h"

namespace { 
  std::string tm_to_string ( const std::tm& tm ) {
    //TODO Do this with <iomanip> when available 
    std::string date_str;
    date_str.resize(11);
    strftime(&date_str[0], date_str.size(), "%Y-%m-%d", &tm);
    return date_str;
  };
};

std::unique_ptr<sql::Connection> Dfp::Database::init_conn ( 
    const std::string& host, const std::string& user, 
    const std::string& passwd ) const 
{
    driver->threadInit();
    std::unique_ptr<sql::Connection> conn ( 
        driver->connect(host, user, passwd) );
    std::unique_ptr<sql::Statement> stmt (
        conn->createStatement ());
    std::unique_ptr< sql::ResultSet> res (stmt->executeQuery (
          "SELECT SUBSTRING_INDEX(CURRENT_USER(),\'@\', 1)"));
    res->next();
    if (res->getString(1).compare("denet")==0)
      return conn;
    throw Dfp::Exception("No user denet registered", Dfp::EXCEPTION_NO_USER);
}

Dfp::Database::Database ( const std::string& host, const std::string& user, 
    const std::string& passwd ) : driver (sql::mysql::get_driver_instance()),
    conn ( init_conn (host, user, passwd) ) { }

Dfp::Database::~Database()
{
  driver->threadEnd();
}

Dfp::Company Dfp::Database::get_company_from_cvm ( int cvm ) const 
{
  std::unique_ptr< sql::Statement> stmt ( conn->createStatement());
  stmt->execute ( "USE denet" );
  stmt.reset(NULL);
  std::unique_ptr<sql::PreparedStatement> prep_stmt (
      conn->prepareStatement( "SELECT ticker, class FROM tickers WHERE cvm = ?"));
  prep_stmt->setInt (1,cvm);
  std::unique_ptr< sql::ResultSet> res ( prep_stmt->executeQuery ());
  std::vector<Ticker> tickers_vect;
  Dfp::Ticker ticker;
  while ( res->next() ) {
    ticker.code = std::string ( res->getString("ticker") ); 
    ticker.type = static_cast<TickerType> (res->getInt(2));
    tickers_vect.push_back ( ticker );
  }
  prep_stmt.reset( conn->prepareStatement ( 
        "SELECT name, sector, cnpj, segmento, situation, email, address_city,"
        "address_state, address_country FROM companies WHERE cvm = ?"));
  prep_stmt->setInt(1, cvm);
  res.reset ( prep_stmt->executeQuery());
  res->next();
  return Dfp::Company ( cvm, res->getString(1), res->getString(2),
      res->getString(3), tickers_vect, res->getString(4), res->getString(5),
      res->getString(6), res->getString(7), res->getString(8), 
      res->getString(9), *this); 
}

int Dfp::Database::get_cvm_from_ticker_str ( const std::string & str ) const
{
  std::unique_ptr< sql::Statement> stmt ( conn->createStatement());
  stmt->execute ( "USE denet" );
  stmt.reset(NULL);
  std::unique_ptr<sql::PreparedStatement> prep_stmt (
      conn->prepareStatement( "SELECT cvm FROM tickers WHERE ticker = ?") );
  prep_stmt->setString (1,str);
  std::unique_ptr< sql::ResultSet> res ( prep_stmt->executeQuery ());
  if (!res->next() ) return 0;
  return res->getInt(1);
}

int Dfp::Database::last_imported_revision (int cvm, std::tm tm) const
{
  std::unique_ptr< sql::Statement> stmt ( conn->createStatement());
  stmt->execute ( "USE denet" );

  std::string date_str = tm_to_string ( tm );
  std::unique_ptr< sql::ResultSet> res ( stmt->executeQuery ( 
        "SELECT MAX( version ) FROM revisions WHERE cvm = " + 
        std::to_string (cvm) + " AND exercise = \"" + date_str + "\"" ) ); 
  if (!res->next()) return 0;
  return res->getInt(1);
};

void Dfp::Database::add_revision (int cvm, int revision, std::tm tm ) const
{ 
  std::unique_ptr< sql::Statement> stmt ( conn->createStatement());
  stmt->execute ( "USE denet" );
  std::string date_str = tm_to_string ( tm );
  stmt->execute ( "INSERT INTO revisions VALUES (\"" + std::to_string (cvm) + 
      "\", \"" + date_str + "\", \"" + std::to_string (revision ) + "\")" );
  stmt->execute ( "DELETE FROM cvm_" + std::to_string (cvm) + 
      " WHERE number LIKE \"3.99\%\"");
};

void Dfp::Database::delete_exercise (int cvm, std::tm tm ) const
{ 
  std::unique_ptr< sql::Statement> stmt ( conn->createStatement());
  stmt->execute ( "USE denet" );

  std::string date_str = tm_to_string (tm);
  //TODO check for errors
  stmt->execute ( "DELETE FROM cvm_" + std::to_string (cvm) + " WHERE date ="
      " \"" + date_str + "\" ");
  stmt->execute ( "DELETE FROM revisions where exercise = \"" + date_str +
      "\"" );
}

std::tm Dfp::Database::last_imported_exercise ( int cvm ) const
{
  std::unique_ptr< sql::Statement> stmt ( conn->createStatement());
  stmt->execute ( "USE denet" );
  std::unique_ptr< sql::ResultSet> res ( stmt->executeQuery ( 
        "SELECT MAX( exercise ) FROM revisions where cvm=" + 
        std::to_string ( cvm ) ) );
  if (!res->next()) {
    std::string what_arg = "Dfp::Database::last_imported_exercise: "
      "no exercise imported for cvm " + std::to_string (cvm);
    throw Dfp::Exception(what_arg.c_str(), EXCEPTION_NO_EXERCISE);
  }
  std::string test_empty ( res->getString(1) );
  if (test_empty.empty()) {
    std::string what_arg = "Dfp::Database::last_imported_exercise: "
      "no exercise imported for cvm " + std::to_string (cvm);
    throw Dfp::Exception(what_arg.c_str(), EXCEPTION_NO_EXERCISE);
  }
  //TODO: use iomanip get_time when available c++17
  std::tm tm;
  std::stringstream date_str ( res->getString (1) ); 
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
  return tm;
}

Dfp::BalanceType Dfp::Database::get_balance_type_from_account_number(int cvm, 
    std::string account_number ) const
{ 
  std::unique_ptr< sql::Statement> stmt ( conn->createStatement());
  stmt->execute ( "USE denet" );
  stmt.reset(NULL);
  std::unique_ptr< sql::PreparedStatement> prep_stmt ( conn->prepareStatement (
        "SELECT balance_type FROM cvm_" + std::to_string (cvm) + 
        " WHERE number = ? LIMIT 1"));
  prep_stmt->setString (1, account_number);
  std::unique_ptr< sql::ResultSet> res ( prep_stmt->executeQuery ());
  if (! res->next()) return DFP_BALANCE_INVALID;
  return static_cast<BalanceType> ( res->getInt(1) );
}

namespace 
{
  template <class F1, class F2>
    struct overload_set : F1, F2
  {
    overload_set(F1 f1, F2 f2) : F1(f1), F2(f2) {}

    using F1::operator();
    using F2::operator();
  };
  template <class F1, class F2>
    overload_set<F1, F2> overload(F1 f1, F2 f2)
    {
      return overload_set<F1, F2>(f1, f2);
    };
  struct find_ticker
  {
    const Dfp::TickerType type;
    find_ticker(Dfp::TickerType type) : type(type) {};
    bool operator () ( const Dfp::Ticker& ticker ) const
    {
      return ticker.type == type;
    }
  };
};

int Dfp::Database::get_indicator ( int cvm, std::string account_number ) const 
{
  return get_indicator (cvm, account_number,
      last_imported_exercise ( cvm ));
};
int Dfp::Database::get_indicator ( int cvm , std::string account_number,
    std::tm exercise, bool anual , 
    Dfp::FinancialInfoType type ) const 
{
  BalanceType balance_type = get_balance_type_from_account_number ( cvm, 
      account_number );
  switch ( balance_type ) {
    case DFP_BALANCE_INVALID: 
    case DFP_BALANCE_DMPL:
      throw std::invalid_argument ( "Dfp::Database::get_indicator: "
          "invalid BalanceType" );
    case DFP_BALANCE_DRE: 
    case DFP_BALANCE_DRA:
      if ( anual ) {
        if ( exercise.tm_mon == 11 ) goto return_last_value;
        //TODO: For the love of god use c++17 when available!!!!
        time_t timer;
        exercise.tm_hour=12;
        exercise.tm_min=0;
        exercise.tm_sec=0;
        int val = 0;
        for (int i = 0; i < 4 ; i++) {
          val += get_indicator ( cvm, account_number, 
              exercise, false, type);
          exercise.tm_mon -=2; 
          exercise.tm_mday = 0;
          timer = mktime ( &exercise );
          exercise = * localtime (&timer);
        };
        return val;
      }
      else {
        if ( exercise.tm_mon != 11 ) goto return_last_value;
        //TODO: For the love of god use c++17 when available!!!!
        time_t timer;
        exercise.tm_hour=12;
        exercise.tm_min=0;
        exercise.tm_sec=0;
        int val = get_indicator ( cvm, account_number, exercise, 
            true, type);
        for (int i = 0; i < 3 ; i++) {
          exercise.tm_mon -=2; 
          exercise.tm_mday = 0;
          timer = mktime ( &exercise );
          exercise = * localtime (&timer);
          val -= get_indicator ( cvm, account_number, exercise, 
              false, type);
        };
        return val;
      };
      break;
    case DFP_BALANCE_DVA:
    case DFP_BALANCE_DFC_MD:
    case DFP_BALANCE_DFC_MI:
      {
        if ( anual || exercise.tm_mon == 2 || 
            (account_number.compare ( "6.05.02" ) == 0 )) goto return_last_value;
        // Accounts 6.05.?? need to be treated separatedly cause the initial and
        // final values are the results from last trimester. 
        if ( account_number.compare ( "6.05.01" ) == 0 ) {
          //TODO: For the love of god use c++17 when available!!!!
          time_t timer;
          exercise.tm_hour=12;
          exercise.tm_min=0;
          exercise.tm_sec=0;
          exercise.tm_mon -=2; 
          exercise.tm_mday = 0;
          timer = mktime ( &exercise );
          exercise = * localtime (&timer);
          return get_indicator ( cvm, "6.05.02", exercise, true, type);
        };
        //TODO: For the love of god use c++17 when available!!!!
        time_t timer;
        exercise.tm_hour=12;
        exercise.tm_min=0;
        exercise.tm_sec=0;
        int val = get_indicator ( cvm, account_number, exercise, 
            true, type);
        exercise.tm_mon -=2; 
        exercise.tm_mday = 0;
        timer = mktime ( &exercise );
        exercise = * localtime (&timer);
        val -= get_indicator ( cvm, account_number, exercise, true, type);
        return val;
        break;
      }
    case DFP_BALANCE_IF:
    case DFP_BALANCE_BPA:
    case DFP_BALANCE_BPP:
      break;
  }
return_last_value: 
  std::string date_str = tm_to_string ( exercise );

  std::unique_ptr< sql::Statement> stmt ( conn->createStatement());
  stmt->execute ( "USE denet" );
  stmt.reset (NULL);
  std::unique_ptr< sql::PreparedStatement> prep_stmt( conn->prepareStatement(
        "SELECT value FROM cvm_" +std::to_string (cvm)+ 
        " WHERE number = ? AND date = ? AND financial_info_type = ?") );
  prep_stmt->setString (1,  account_number );
  prep_stmt->setDateTime (2,  date_str.c_str() );
  prep_stmt->setInt (3, static_cast<int> (type));

  std::unique_ptr< sql::ResultSet > res ( prep_stmt->executeQuery ( )); 
  if (! res->next() ) { 
    std::string what_arg = "Dfp::Database::get_indicator: No account found: " 
      + account_number + " -- " + date_str; 
    throw Dfp::Exception ( what_arg.c_str(), EXCEPTION_NO_ACCT );
  }
  return res->getInt(1);
};
int Dfp::Database::get_indicator ( int cvm, std::string account_number,
    bool anual, Dfp::FinancialInfoType type) const
{
  return get_indicator ( cvm, account_number, last_imported_exercise (cvm),
      anual , type );
};
int Dfp::Database::get_indicator ( int cvm, std::string account_number, 
    Dfp::FinancialInfoType type ) const
{
  return get_indicator ( cvm, account_number, last_imported_exercise (cvm),
      false, type );
};
float Dfp::Database::get_indicator ( int cvm,  Dfp::Indicator indicator, 
    std::tm exercise, bool anual , 
    Dfp::FinancialInfoType type ) const 
{
  auto f = overload (
      [=] ( Dfp::Indicator ind ) -> float { return get_indicator (cvm, 
        ind, exercise, anual, type ); },
      [=] ( std::string ind ) -> int { return get_indicator (cvm, 
        ind, exercise, anual, type ); } );
  std::unique_ptr< sql::Statement> stmt ( conn->createStatement());
  stmt->execute ( "USE denet" );
  switch ( indicator ) {
    case DFP_INDICATOR_MV: 
      {
        // Should import FRE in order to compute this correctly!
        std::string date_str = tm_to_string ( exercise );
        std::unique_ptr<sql::PreparedStatement> prep_stmt (
            conn->prepareStatement( "SELECT CONCAT (\"1.90.\","
              " SUBSTR( number, 6, 2)), "
              " value FROM cvm_" + std::to_string (cvm) + 
              " WHERE number LIKE \"1.91.%\" AND value != 0 AND date = ?" ) );
        prep_stmt->setDateTime (1, date_str.c_str());
        std::unique_ptr<sql::ResultSet> res ( prep_stmt-> executeQuery());
        if ( res->rowsCount() == 0 ) {
          Dfp::Company company = get_company_from_cvm (cvm);
          std::vector<Dfp::Ticker> tickers = company.get_tickers();
          if ( std::find_if ( tickers.begin(), tickers.end(), find_ticker (
                  DFP_TICKER_PN ) ) != tickers.end() ) 
            return 
              (get_indicator( cvm, "1.89.01", exercise) * 
               get_indicator( cvm, "1.90.03", exercise) + 
               get_indicator( cvm, "1.89.02", exercise) *
               get_indicator( cvm, "1.90.04", exercise) ) / 100;
          else if ( std::find_if ( tickers.begin(), tickers.end(), 
                find_ticker (DFP_TICKER_PNA ) ) != tickers.end() )
            return 
              (get_indicator( cvm, "1.89.01", exercise) * 
              get_indicator( cvm, "1.90.03", exercise) + 
              get_indicator( cvm, "1.89.02", exercise) *
              get_indicator( cvm, "1.90.05", exercise) ) / 100;
          else return 
              get_indicator( cvm, "1.89.01", exercise) * 
              get_indicator( cvm, "1.90.03", exercise) /100;
        }
        else {
          int ret = 0;
          while (res->next())
            ret += 
              get_indicator(cvm,res->getString(1),exercise)*res->getInt(2)/100;
          return ret;
        };
      }
    case DFP_INDICATOR_PE:
      {
        float ret = f(DFP_INDICATOR_MV)/f("3.11"); 
        if ( anual ) return ret;
        else return ret/4;
      }
    case DFP_INDICATOR_EBIT: 
      return f("3.05");
    case DFP_INDICATOR_PEBIT:
      {
        float ret = f(DFP_INDICATOR_MV)/f("3.05");
        if (anual) return ret;
        else return ret/4;
      }
    case DFP_INDICATOR_PSR:
      {
        float ret = f(DFP_INDICATOR_MV)/f("3.01");
        if (anual) return ret;
        else return ret/4;
      }
    case DFP_INDICATOR_PASSET: return f(DFP_INDICATOR_MV)/f("1");
    case DFP_INDICATOR_WORKCAP: return f("1.01")-f("2.01");
    case DFP_INDICATOR_PWORKCAP: 
                                return f(DFP_INDICATOR_MV)/f(DFP_INDICATOR_WORKCAP);
    case DFP_INDICATOR_PNETLIQASSET: 
                                return f(DFP_INDICATOR_MV)/(f("1.01") - f("1.01.04"));
    case DFP_INDICATOR_LIAB: return f("2.01.04") + f("2.02.01");
    case DFP_INDICATOR_NETLIAB: return f(DFP_INDICATOR_LIAB) - f("1.01.01");
    case DFP_INDICATOR_EV: return f(DFP_INDICATOR_MV) + 
                           f(DFP_INDICATOR_NETLIAB);
    case DFP_INDICATOR_EVEBIT: 
                           {
                             float ret = f(DFP_INDICATOR_EV)/f(DFP_INDICATOR_EBIT);
                             if (anual) return ret;
                             else return ret/4;
                           }
    case DFP_INDICATOR_EARNSHARE: return (float) f("3.11")/get_indicator(cvm, 
                                      "1.89.03", exercise);
    case DFP_INDICATOR_VPA: return (float) f("2.03")/get_indicator(cvm, 
                                "1.89.03", exercise);
    case DFP_INDICATOR_MB: return (float) f("3.03")/f("3.01");
    case DFP_INDICATOR_MEBIT: return f(DFP_INDICATOR_EBIT)/f("3.01");
    case DFP_INDICATOR_ML: return (float) f("3.11")/f("3.01");
    case DFP_INDICATOR_EBITASSETS: 
                           {
                             float ret = f(DFP_INDICATOR_EBIT)/ f("1");
                             if (anual) return ret;
                             else return 4*ret;
                           }
    case DFP_INDICATOR_ROE:
                           {
                             float ret = (float) f("3.11")/f("2.03");
                             if (anual) return ret;
                             else return 4*ret;
                           }
    case DFP_INDICATOR_ROIC:
                           { 
                             float ret = 
                               f(DFP_INDICATOR_EBIT)/ (f("1") - f("1.01.01") - f("2.01.02"));
                             if (anual) return ret;
                             else return 4*ret;
                           }
    case DFP_INDICATOR_LIQCORR: return (float) f("1.01")/f("2.01");
    case DFP_INDICATOR_DIVBRPL: return f(DFP_INDICATOR_LIAB)/f("2.03");
    case DFP_INDICATOR_PVP: return f(DFP_INDICATOR_MV)/f("2.03");
    case DFP_INDICATOR_DY: 
                            { 
                              float ret = (f("7.08.04.01") + f("7.08.04.02"))/f(DFP_INDICATOR_MV);
                              if (anual) return ret;
                              else return ret*4;
                            }
    case DFP_INDICATOR_PL: return f("2.03");
    case DFP_INDICATOR_REVENUES: return f("3.01");
    case DFP_INDICATOR_EARNINGS: return f("3.11");
    case DFP_INDICATOR_COSTS: return f("3.02");
    case DFP_INDICATOR_BRUTO: return f("3.03");
    case DFP_INDICATOR_FINANCIAL: return f("3.06");
    default:
                           break;
  }
  return 0;
};
float Dfp::Database::get_indicator ( int cvm,  Dfp::Indicator indicator, 
    bool anual, Dfp::FinancialInfoType type) const
{
  return get_indicator (cvm, indicator, last_imported_exercise (cvm), anual, 
      type);
};
float Dfp::Database::get_indicator ( int cvm, Dfp::Indicator indicator, 
    Dfp::FinancialInfoType type ) const
{
  return get_indicator (cvm, indicator, last_imported_exercise (cvm), false,
      type );
};

float Dfp::Database::get_indicator ( int cvm, Dfp::Indicator indicator ) const
{
  return get_indicator ( cvm, indicator, last_imported_exercise (cvm) );
};
std::vector<Dfp::CvmUrl> Dfp::Database::available_for_download (int cvm, 
    CvmFileType filetype) const 
{
  std::vector<CvmUrl> cvmurl;
  switch ( filetype ) {
    case DFP_CVM_FILE_DFP:
    case DFP_CVM_FILE_ITR:
    case DFP_CVM_FILE_FRE: 
      cvmurl = Cvm::available_for_download ( cvm, filetype);
      break;
    default: throw std::invalid_argument (
                 "Dfp::Database::available_for_download: invalid CvmFileType" ); 
             break;
  }
  for ( auto it = cvmurl.begin(); it != cvmurl.end();) {
    //TODO: use iomanip get_time when available c++17
    //TODO: this fails for fre
    std::tm tm;
    std::stringstream date_str ( it->date_str ); 
    date_str >> tm.tm_mday;
    date_str.ignore();
    date_str >> tm.tm_mon;
    date_str.ignore();
    date_str >> tm.tm_year; 
    tm.tm_year -= 1900;
    tm.tm_mon--;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    debug_log ( "Database::available_for_download Revision: " + 
        std::to_string (it->revision ) + 
        " last imported: " + 
        std::to_string( last_imported_revision(cvm,tm) ) +
        " date: "+ 
        it->date_str + "\n");
    if ( it->revision <= last_imported_revision (cvm, tm ) ) 
      it = cvmurl.erase (it); 
    else
      ++it;
  }
  return cvmurl;
};

void Dfp::Database::create_table (int cvm ) const
{ 
  std::unique_ptr< sql::Statement> stmt ( conn->createStatement());
  stmt->execute ( "USE denet" );
  stmt->execute ( "CREATE TABLE IF NOT EXISTS `cvm_" + std::to_string (cvm) + 
      "` (number VARCHAR (15), name VARCHAR (100), date DATE, "
      "balance_type INT, financial_info_type INT, value INT, "
      "comments VARCHAR(100), PRIMARY KEY ( number, date, financial_info_type"
      ", comments ) )");
};

void Dfp::Database::import_account ( int cvm, 
    const DatabaseAccount& acct ) const 
{ 
  std::unique_ptr< sql::Statement> stmt ( conn->createStatement());
  stmt->execute ( "USE denet" );
  stmt.reset (NULL);
  std::unique_ptr<sql::PreparedStatement> prep_stmt (
      conn->prepareStatement( "INSERT INTO cvm_" + std::to_string (cvm) + 
        " VALUES ( ?, ?, ?, ?, ?, ? , ? )" ) );
  debug_log ( "About to import values " + acct.number + ", " + acct.name + 
      ", " + acct.date + "...\n" );
  prep_stmt->setString (1, acct.number);
  prep_stmt->setString (2, acct.name);
  prep_stmt->setDateTime (3, acct.date.c_str() );
  prep_stmt->setInt (4, static_cast<int> ( acct.balance_type ));
  prep_stmt->setInt (5, static_cast<int> (acct.financial_info_type ) );
  prep_stmt->setInt (6, acct.value);
  prep_stmt->setString (7, acct.comments);
  prep_stmt->execute();
}


std::string Dfp::Database::get_comment (int cvm, 
    const std::string &account_number, 
    std::tm date, 
    Dfp::FinancialInfoType type) const
{
  std::unique_ptr< sql::Statement> stmt ( conn->createStatement());
  stmt->execute ( "USE denet" );
  std::unique_ptr<sql::PreparedStatement> prep_stmt (
      conn->prepareStatement( "SELECT comments from cvm_" + std::to_string(cvm)
        + " WHERE number=? AND date=? AND financial_info_type=?"));
  prep_stmt->setString (1,account_number.c_str());
  std::string date_str=tm_to_string(date);
  prep_stmt->setString (2,date_str.c_str());
  prep_stmt->setInt (3,static_cast<int>(type));
  std::unique_ptr< sql::ResultSet> res ( prep_stmt->executeQuery ());
  if (!res->next()) 
    return std::string();
  return res->getString(1);
}





