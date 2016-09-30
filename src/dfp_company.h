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
#ifndef DFP_COMPANY
#define DFP_COMPANY
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include "dfp_database.h"
/// \file
namespace Dfp {
  /// \ingroup Enumerations
  /// 
  /// Types of tickers
  enum TickerType {
    DFP_TICKER_SUBS_ON = 1,
    DFP_TICKER_SUBS_PN,
    DFP_TICKER_ON,
    DFP_TICKER_PN,
    DFP_TICKER_PNA, 
    DFP_TICKER_PNB, 
    DFP_TICKER_PNC,
    DFP_TICKER_PND,
    DFP_TICKER_REC_ON, 
    DFP_TICKER_REC_PN,
    DFP_TICKER_UNT,
    DFP_TICKER_ETF,
  };
  struct Ticker {
    std::string  code;
    TickerType type;
  };
  class Company {
      int cvm;
      std::string name;
      std::string sector; ///< e.g. "Metalurgia"
      std::string cnpj;
      /// All negotiated tickers for this company
      std::vector<Ticker> tickers;
      std::string bovespa_segment; ///< e.g. "NM"
      std::string situation;
      std::string email; 
      std::string address_city;
      std::string address_state;
      std::string address_country;

      const std::shared_ptr<Dfp::Database> conn;
      
    public:
      Company ( int cvm_, const std::shared_ptr<Dfp::Database> conn_ );
      Company ( std::string ticker_code , 
                const std::shared_ptr<Dfp::Database> conn_ );
      Company (int cvm_, const std::string name_, const std::string sector_,
          const std::string cnpj_, const std::vector<Ticker> tickers_, 
          const std::string bovespa_segment_, const std::string situation_,
          const std::string email_,
          const std::string address_city_, const std::string address_state_, 
          const std::string address_country_, 
          std::shared_ptr<Dfp::Database> conn_ ) : cvm ( cvm_ ), 
      sector (sector_), cnpj (cnpj_), tickers (tickers_), 
      bovespa_segment (bovespa_segment_), situation (situation_), 
      email (email_),
      address_city (address_city_), address_country (address_country_), 
      conn ( std::move (conn_)) { };

      Dfp::Ticker get_ticker_on ();
      int get_cvm () { return cvm; };
      std::vector<Ticker> get_tickers () { return tickers; };

      void create_table ();
      int last_imported_revision ( std::tm tm ); 
      std::tm last_imported_exercise ();
      void add_revision ( int revision, std::tm tm );
      void delete_exercise ( std::tm tm );


      float get_indicator ( Dfp::Indicator indicator );
      float get_indicator ( Dfp::Indicator indicator, std::tm exercise,
          bool anual = true, 
          Dfp::FinancialInfoType type = DFP_FINANCIAL_INFO_CONSOLIDATED);
      float get_indicator ( Dfp::Indicator indicator, bool anual);
      float get_indicator ( Dfp::Indicator indicator, 
          Dfp::FinancialInfoType type );
  };
}
#endif
