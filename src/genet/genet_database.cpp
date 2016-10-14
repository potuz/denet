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
#include "genet_database.h"
#include "cppconn/statement.h"	// sql::Statement
#include <QString>
#include <QStringList>

namespace Genet {
  GenetDatabase::GenetDatabase (QString host, QString user, QString password)
    : Dfp::Database (host.toStdString(), user.toStdString(), 
        password.toStdString()) {}

  int GenetDatabase::get_indicator (int cvm, QString account_number, bool anual,
      Dfp::FinancialInfoType type) const 
  { 
    return Dfp::Database::get_indicator (cvm, account_number.toStdString(), 
        anual, type);
  }

  void GenetDatabase::tickers(QStringList &codes) const 
  {
    std::unique_ptr<sql::Statement> stmt ( conn->createStatement());
    stmt->execute ( "USE denet" );

    std::unique_ptr< sql::ResultSet> res(stmt->executeQuery ( 
          "SELECT ticker FROM tickers")); 
    while (res->next()) 
      codes.append (res->getString(1).c_str());
  }

  int GenetDatabase::get_cvm_from_ticker_str (const QString &str) const
  {
    return Dfp::Database::get_cvm_from_ticker_str (str.toStdString());
  }
}
