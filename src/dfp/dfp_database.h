/*  This file is part of denet. 
 *  denet is a tool to study Bovespa-listed companies. 
 *
 *  Copyright (c) 2016 potuz@potuz.net
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
#ifndef DFP_DATABASE_INCLUDED
#define DFP_DATABASE_INCLUDED
#include <string>
#include <memory>
#include <ctime>
#include <vector>
#include "dfp_cvm_connection.h"
#include "cppconn/connection.h" // sql::Connection
///\file
namespace Dfp {
  /// \ingroup Enumerations
  /// \headerfile dfp_database.h "dfp_database.h"
  /// Type of result balancesheet.
  enum BalanceType {
    DFP_BALANCE_INVALID,
    DFP_BALANCE_IF,
    DFP_BALANCE_BPA,
    DFP_BALANCE_BPP,
    DFP_BALANCE_DRE,
    DFP_BALANCE_DRA,
    DFP_BALANCE_DFC_MD,
    DFP_BALANCE_DFC_MI,
    DFP_BALANCE_DMPL,
    DFP_BALANCE_DVA
  };
  /// \ingroup Enumerations
  /// Whether account is consolidated or individual
  enum FinancialInfoType : unsigned int {
    DFP_FINANCIAL_INFO_INDIVIDUAL = 1,
    DFP_FINANCIAL_INFO_CONSOLIDATED,
  };
  struct DatabaseAccount {
    std::string number;
    std::string name;
    std::string date;
    std::string comments;
    BalanceType balance_type;
    FinancialInfoType financial_info_type;
    int value;
  };
  class Company;
  struct Ticker;
  /// \ingroup Enumerations
  /// The supported indicators
  enum Indicator : unsigned int {
    DFP_INDICATOR_INVALID = 0,
    DFP_INDICATOR_MV,
    DFP_INDICATOR_PE,
    DFP_INDICATOR_EBIT,
    DFP_INDICATOR_PEBIT,
    DFP_INDICATOR_PSR,
    DFP_INDICATOR_PASSET,
    DFP_INDICATOR_WORKCAP,
    DFP_INDICATOR_PWORKCAP,
    DFP_INDICATOR_NETLIQASSET,
    DFP_INDICATOR_PNETLIQASSET,
    DFP_INDICATOR_LIAB,
    DFP_INDICATOR_NETLIAB,
    DFP_INDICATOR_EV,
    DFP_INDICATOR_EVEBIT,
    DFP_INDICATOR_EARNSHARE,  ///<earnings per share
    DFP_INDICATOR_VPA,
    DFP_INDICATOR_MB,
    DFP_INDICATOR_MEBIT,
    DFP_INDICATOR_ML,
    DFP_INDICATOR_EBITASSETS,
    DFP_INDICATOR_ROE,
    DFP_INDICATOR_ROIC,
    DFP_INDICATOR_LIQCORR,
    DFP_INDICATOR_DIVBRPL,
    DFP_INDICATOR_PVP,
    DFP_INDICATOR_DY, 
    DFP_INDICADOR_PL
  };

  /// Holds a connection to the MySQL database
  ///
  /// This is the only class that is permitted to write/query the database
  /// directly. 
  class Database 
  {
    protected:
    const std::unique_ptr<sql::Connection> conn;

    public:
      Database ( const std::string& host, const std::string& user, 
          const std::string& passwd );

      void import_account ( int cvm, const DatabaseAccount& acct ) const ;
      Dfp::Company get_company_from_cvm ( int cvm ) const;
      int get_cvm_from_ticker_str ( const std::string & str ) const;
      Dfp::Company get_company_from_ticker ( const Dfp::Ticker& ticker ) const;
      void setup_company ( Dfp::Company& company, int cvm ) const ;
      void setup_company ( Dfp::Company& company, 
          const std::string& ticker_code )const;
      void setup_company ( Dfp::Company& company, 
          const Dfp::Ticker& ticker ) const;
      void setup_company ( Dfp::Company& company ) const;

      std::string get_ticker_on_from_cvm ( int cvm );
      std::vector<std::string> get_tickers_from_cvm ( int cvm );

      int last_imported_revision ( int cvm, std::tm tm ) const;
      std::tm last_imported_exercise ( int cvm ) const;
      void add_revision (int cvm, int revision, std::tm tm ) const;
      void delete_exercise (int cvm, std::tm tm ) const;

      void create_table ( int cvm ) const;

      BalanceType get_balance_type_from_account_number ( int cvm, 
          std::string account_number ) const;
      /// \brief Returns the value of the given account. 
      /// 
      /// It does not return the value
      /// accounts with Dfp::BalanceType DFP_BALANCE_DMPL or 
      /// DFP_BALANCE_INVALID in
      /// which case it throws std::invalid_arguments. 
      int get_indicator ( int cvm , std::string account_number,
          std::tm exercise, bool anual = false, 
          Dfp::FinancialInfoType type = DFP_FINANCIAL_INFO_CONSOLIDATED) const;
      int get_indicator ( int cvm, std::string account_number ) const;
      int get_indicator ( int cvm, std::string account_number, bool anual, 
          Dfp::FinancialInfoType type = DFP_FINANCIAL_INFO_CONSOLIDATED) const;
      int get_indicator ( int, std::string account_number, 
          Dfp::FinancialInfoType type ) const;

      float get_indicator ( int cvm, Dfp::Indicator indicator ) const ;
      float get_indicator ( int cvm , Dfp::Indicator indicator, 
          std::tm exercise, bool anual = false, 
          Dfp::FinancialInfoType type = DFP_FINANCIAL_INFO_CONSOLIDATED) const;
      float get_indicator ( int cvm, Dfp::Indicator indicator, bool anual, 
          Dfp::FinancialInfoType type = DFP_FINANCIAL_INFO_CONSOLIDATED) const;
      float get_indicator ( int cvm, Dfp::Indicator indicator, 
          Dfp::FinancialInfoType type ) const;

      std::vector<CvmUrl> available_for_download (int cvm, 
          CvmFileType filetype) const;
   };
};
#endif
