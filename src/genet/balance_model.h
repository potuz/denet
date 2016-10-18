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
#ifndef BALANCE_MODEL_INCLUDED
#define BALANCE_MODEL_INCLUDED
/// \file
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QDate>
#include "dfp/dfp_database.h"
#include "balance_item.h"

namespace Genet { 
  class GenetDatabase;

  class BalanceModel : public QAbstractItemModel
  {
    Q_OBJECT

    public:
      BalanceModel(int cvm, 
          const GenetDatabase &conn, 
          Dfp::BalanceType balanceType,
          bool anual=true, 
          QDate date=QDate(), 
          Dfp::FinancialInfoType type=Dfp::DFP_FINANCIAL_INFO_CONSOLIDATED, 
          QObject *parent=0);

      ~BalanceModel();

      Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
      QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
      QVariant headerData(int section, Qt::Orientation orientation, 
          int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

      QModelIndex index(int row, int column, const QModelIndex &parent = 
          QModelIndex()) const Q_DECL_OVERRIDE;

      QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

      int rowCount(const QModelIndex &parent =
          QModelIndex()) const Q_DECL_OVERRIDE;
      int columnCount(const QModelIndex &parent = 
          QModelIndex()) const Q_DECL_OVERRIDE;

      bool setData(const QModelIndex &index, const QVariant &value, 
          int role = Qt::EditRole);

    private slots:
      void setCvm(int);
      void setAnual(bool);
      void setExercise(QDate);
      void setBalanceType(Dfp::BalanceType);
      void setType(Dfp::FinancialInfoType);

    private:
      int cvm;
      const GenetDatabase &conn;
      bool anual;
      QDate date;
      Dfp::BalanceType balanceType;
      Dfp::FinancialInfoType type;
      BalanceItem *getItem(const QModelIndex &index) const;
      BalanceItem *rootItem;
      void setupModel ();
  };
}
#endif
