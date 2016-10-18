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
#include "cppconn/prepared_statement.h"
#include "dfp/dfp_exception.h"
#include <QString>
#include <QStringList>
#include <QDate>
#include <QVector>
#include <QVariant>

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

  QVector<QVector<QVariant>> GenetDatabase::process_account_list(int cvm,
      QDate date,
      bool anual, Dfp::FinancialInfoType type, 
      const std::unique_ptr<sql::ResultSet> res) const
  {
    //TODO use <iomanip> get_time when available
    QDate previousItr = date.addMonths(-3);
    std::tm tmPreviousItr;
    tmPreviousItr.tm_year = previousItr.year()-1900;
    tmPreviousItr.tm_mon = previousItr.month()-1;
    tmPreviousItr.tm_mday = previousItr.daysInMonth();
    tmPreviousItr.tm_hour=0;
    tmPreviousItr.tm_min=0;
    tmPreviousItr.tm_sec=0;

    std::tm tmDate;
    tmDate.tm_year = date.year()-1900;
    tmDate.tm_mon = date.month()-1;
    tmDate.tm_mday = date.day();
    tmDate.tm_hour=0;
    tmDate.tm_min=0;
    tmDate.tm_sec=0;

    std::tm tmPreviousYear = tmDate;
    --tmPreviousYear.tm_year;

    QVector<QVector<QVariant>> result = QVector<QVector<QVariant>>();
    while(res->next())
    {
      QVector<QVariant> inner;
      try {
        inner << res->getString(1).c_str();
        inner << res->getString(2).c_str();
        inner << QString("%L1").arg(get_indicator(cvm,res->getString(1),
              tmDate,anual,type));
        inner << QString("%L1").arg(get_indicator(cvm,res->getString(1),
              tmPreviousItr,anual,type));
        inner << QString("%L1").arg(get_indicator(cvm,res->getString(1),
            tmPreviousYear, anual,type));
        inner << QString::fromStdString(get_comment(cvm,
                res->getString(1),tmDate,type));
        result << inner;
      } catch ( Dfp::Exception &e )
      {
      }
    }
    return result;
  }

  QVector<QVector<QVariant>> GenetDatabase::get_account_list(int cvm, QDate date, 
          Dfp::BalanceType balanceType, bool anual, 
          Dfp::FinancialInfoType type) const
  {
    std::unique_ptr<sql::Statement> stmt ( conn->createStatement());
    stmt->execute ( "USE denet" );

    std::unique_ptr<sql::PreparedStatement> prep_stmt (
      conn->prepareStatement( "SELECT number,name FROM cvm_"+std::to_string(cvm)+ 
        " WHERE number LIKE \"_.__\" AND balance_type=? GROUP BY number") );
    prep_stmt->setInt(1,static_cast<int> (balanceType));
    std::unique_ptr< sql::ResultSet> res ( prep_stmt->executeQuery ());
    auto vector = process_account_list(cvm,date,anual, type, std::move(res));
    if (balanceType==Dfp::DFP_BALANCE_BPA)
    {
      vector << QVector<QVariant>({"1.89"}) << QVector<QVariant>({"1.90"});
      vector << QVector<QVariant>({"1.91"});
    }
    return vector;
  }

  QVector<QVector<QVariant>> GenetDatabase::get_account_list(int cvm, QDate date,
          QString number, bool anual,
          Dfp::FinancialInfoType type) const
  {
    std::unique_ptr<sql::Statement> stmt ( conn->createStatement());
    stmt->execute ( "USE denet" );
    std::unique_ptr<sql::PreparedStatement> prep_stmt (
      conn->prepareStatement( "SELECT number,name FROM cvm_"+std::to_string(cvm)+ 
        " WHERE number LIKE ? GROUP BY number") );
    number.append(".__");
    prep_stmt->setString(1,number.toStdString().c_str());
    std::unique_ptr< sql::ResultSet> res ( prep_stmt->executeQuery ());
    return process_account_list(cvm,date,anual, type, std::move(res));
  }

  QDate Genet::GenetDatabase::last_imported_qdate(int cvm) const
  { 
    std::tm tm = last_imported_exercise(cvm);
    return QDate(tm.tm_year + 1900, tm.tm_mon +1, tm.tm_mday);
  }

  bool Genet::GenetDatabase::setValue(int cvm, const QString &number, 
      const QString &date, Dfp::FinancialInfoType type, QVariant value) const
  {
    std::unique_ptr<sql::Statement> stmt ( conn->createStatement());
    stmt->execute ( "USE denet" );
    if (value.type() == QVariant::Int)
    {
      std::unique_ptr<sql::PreparedStatement> prep_stmt (
          conn->prepareStatement( "UPDATE cvm_"+std::to_string(cvm)+ 
            " SET value=? WHERE number=? AND date=? AND financial_info_type=?"));
      prep_stmt->setInt(1,value.toInt());
      prep_stmt->setString(2,number.toStdString().c_str());
      prep_stmt->setString(3,date.toStdString().c_str());
      prep_stmt->setInt(4,static_cast<int>(type));
      return prep_stmt->execute(); 
    } else if (value.type() == QVariant::String)
    {
      std::unique_ptr<sql::PreparedStatement> prep_stmt (
          conn->prepareStatement( "UPDATE cvm_"+std::to_string(cvm)+ 
            " SET comments=? WHERE number=? AND date=? AND financial_info_type=?"));
      prep_stmt->setString(1,value.toString().toStdString().c_str());
      prep_stmt->setString(2,number.toStdString().c_str());
      prep_stmt->setString(3,date.toStdString().c_str());
      prep_stmt->setInt(4,static_cast<int>(type));
      return prep_stmt->execute(); 
    }
    return false;
  }

  QStringList Genet::GenetDatabase::get_exercise_list(int cvm) const
  {
    std::unique_ptr<sql::Statement> stmt ( conn->createStatement());
    stmt->execute ( "USE denet" );
    std::unique_ptr<sql::PreparedStatement> prep_stmt (
        conn->prepareStatement( "SELECT exercise FROM revisions WHERE cvm=? "
          "GROUP BY exercise"));
    prep_stmt->setInt(1,cvm);
    std::unique_ptr< sql::ResultSet> res ( prep_stmt->executeQuery ());
    QStringList vector;
    while (res->next())
      vector.push_front(QString(res->getString(1).c_str()));
    return vector;
  }
}
