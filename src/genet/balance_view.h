/*  balance_view.h 
 * 
 *  This file is part of denet. 
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
#ifndef BALANCE_VIEW_H
#define BALANCE_VIEW_H
#include <QtWidgets>
#include "genet_database.h"
/// \file

namespace Genet {
  class BalanceView : public QWidget
  {
    Q_OBJECT

    public:
      BalanceView (int cvm, 
          const GenetDatabase &conn, 
          Dfp::BalanceType balanceType, 
          bool anual = true,
          QDate date= QDate(), 
          Dfp::FinancialInfoType type = Dfp::DFP_FINANCIAL_INFO_CONSOLIDATED, 
          QWidget *parent=0);
    public slots:
      void setCvm(int);
      void setType(Dfp::FinancialInfoType);
      void setDate(QDate);
      void setAnual(bool);
      void setBalanceType(Dfp::BalanceType);


    signals:
      void changedCvm(int);
      void changedType(Dfp::FinancialInfoType);
      void changedDate(QDate);
      void changedAnual(bool);
      void changedBalanceType(Dfp::BalanceType);

    private:
      QTreeView *view;
  };
}

#endif
