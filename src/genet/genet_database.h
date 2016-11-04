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
#ifndef GENET_DATABASE_INCLUDED
#define GENET_DATABASE_INCLUDED
///\file
#include <QtGlobal>
#include "dfp/dfp_database.h"

QT_BEGIN_NAMESPACE
class QString;
class QStringList;
class QDate;
class QVariant;
QT_END_NAMESPACE

namespace sql {
  class ResultSet;
}

namespace Genet {
  class GenetDatabase : public Dfp::Database
  {
    public:
      GenetDatabase(QString host, QString user, QString password);

      using Dfp::Database::get_indicator;
      int get_indicator (int cvm, QString account_number, bool anual,
          Dfp::FinancialInfoType type) const;
      void tickers(QStringList &codes) const;
      using Dfp::Database::get_cvm_from_ticker_str;
      int get_cvm_from_ticker_str (const QString &str) const;

      QVector<QVector<QVariant>> get_account_list(int cvm, QDate date, 
          Dfp::BalanceType balanceType, bool anual = true, 
          Dfp::FinancialInfoType type = Dfp::DFP_FINANCIAL_INFO_CONSOLIDATED)const;
      QVector<QVector<QVariant>> get_account_list(int cvm, QDate date,
          QString number, bool anual=true,
          Dfp::FinancialInfoType type = Dfp::DFP_FINANCIAL_INFO_CONSOLIDATED)const;
      
      bool setValue(int cvm, const QString &number, const QString &date, 
          Dfp::FinancialInfoType, QVariant value) const;

      QStringList get_exercise_list(int cvm) const;
      QDate last_imported_qdate(int cvm) const;
      template<class Ind>
        QList<qreal> getSeries(int cvm, bool anual, 
            Dfp::FinancialInfoType type, const Ind &ind) const;
    private:
      QVector<QVector<QVariant>> process_account_list(int cvm, QDate date,
      bool anual, Dfp::FinancialInfoType type, 
      const std::unique_ptr<sql::ResultSet> res) const;
  };
}

#include "genet_database_getseries.h"
#endif
