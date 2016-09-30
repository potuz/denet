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
#include "dfp_utils.h"
#include "mysql_driver.h"
#include "cppconn/statement.h"	// sql::Statement
#include "config.h"

void Dfp::debug_log ( const std::string& str ) { 
  #ifndef NDEBUG
  std::cerr << str;
  #endif
}
void Dfp::setup_dbase ( const std::string & user, const std::string &passwd,
    const std::string &host, const std::string &denetpwd ) {
  sql::Driver *driver (sql::mysql::get_driver_instance()); 
  std::unique_ptr<sql::Connection> conn ( 
      driver->connect(host, user, passwd) );
  std::unique_ptr<sql::Statement> stmt (
      conn->createStatement ());
  std::unique_ptr< sql::ResultSet> res (stmt->executeQuery (
        "SHOW GRANTS FOR " + user + "@" + host) ) ;
  stmt->execute ("CREATE DATABASE denet");
  stmt->execute ("USE denet");
  stmt->execute ("CREATE TABLE companies ( cvm INT NOT NULL, "
      "name VARCHAR(100) DEFAULT NULL, "
      "sector VARCHAR(100) DEFAULT NULL, "
      "cnpj CHAR(14) DEFAULT NULL, "
      "segmento CHAR(2) DEFAULT NULL, "
      "situation VARCHAR(30) DEFAULT NULL, "
      "email VARCHAR(30) DEFAULT NULL, "
      "address_city VARCHAR(30) DEFAULT NULL, "
      "address_state CHAR (2) DEFAULT NULL, "
      "address_country VARCHAR(30) DEFAULT NULL, "
      "PRIMARY KEY (cvm) )");
  stmt->execute ( "CREATE TABLE revisions ( cvm INT NOT NULL, "
      "exercise DATE, "
      "version INT NOT NULL, " 
      "PRIMARY KEY (cvm, exercise) )" );
  stmt->execute ( "CREATE TABLE tickers ( cvm INT NOT NULL, "
      "ticker VARCHAR(12) DEFAULT NULL, "
      "class INT NOT NULL, "
      "PRIMARY KEY (cvm, class) )" );
  stmt->execute ( "LOAD DATA LOCAL INFILE \'"
      DATAFILE_PATH "companies.csv\'"
      " INTO TABLE companies FIELDS TERMINATED BY \',\'"
      " OPTIONALLY ENCLOSED BY \'\"\'"
      " ESCAPED BY \'\\\\\'"
      " LINES TERMINATED BY \'\\n\'" );
  stmt->execute ( "LOAD DATA LOCAL INFILE \'"
      DATAFILE_PATH "tickers.csv\'"
      " INTO TABLE tickers FIELDS TERMINATED BY \',\'"
      " OPTIONALLY ENCLOSED BY \'\"\'"
      " ESCAPED BY \'\\\\\'"
      " LINES TERMINATED BY \'\\n\'" );
  stmt->execute ( "CREATE USER denet@" + host + " IDENTIFIED BY \'" + 
      denetpwd + "\'");
  stmt->execute ("GRANT ALL ON denet.* TO denet@" + host );

} 



