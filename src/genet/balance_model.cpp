/*  balance_model.cpp 
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
#include "dfp/dfp_exception.h"
#include "balance_model.h"
#include "genet_database.h"

void Genet::BalanceModel::setupModel ()
{
  if (!date.isValid()) 
  {
    try { 
      date = conn.last_imported_qdate(cvm);
    } catch ( Dfp::Exception &e ) {
      return;
    }
  }  
  
  beginResetModel();
  delete rootItem;

  QVector<QVariant> rootData;
  rootData << tr("Numero") << tr("Conta") << date.toString("yyyy-MM-dd");
  QDate previous = date.addMonths(-3);
  previous = QDate(previous.year(), previous.month(), previous.daysInMonth());
  rootData << previous.toString("yyyy-MM-dd");
  previous = date.addYears(-1);
  rootData << previous.toString("yyyy-MM-dd") << tr("Comentários");

  rootItem = new BalanceItem(rootData);
  QVector<QVector<QVariant>> topAccts = conn.get_account_list(cvm, date, 
      balanceType, anual, type);
  for (int i =0 ; i < topAccts.size(); ++i)
  {
    BalanceItem *topChild = rootItem->insertChild (topAccts.at(i));
    QVector<QVector<QVariant>> innerAccts = conn.get_account_list(
        cvm,date,topAccts.at(i).at(0).toString(),anual, type);
    for (int i=0; i<innerAccts.size(); ++i)
      topChild->insertChild(innerAccts.at(i));
  }
  endResetModel();
}

Genet::BalanceModel::BalanceModel(int cvm, const GenetDatabase &conn, 
    Dfp::BalanceType balanceType, bool anual, QDate date_,  
    Dfp::FinancialInfoType type,  QObject *parent) 
  : QAbstractItemModel(parent), cvm(cvm), conn(conn), anual(anual), 
  balanceType(balanceType), type(type)
{
  if (date_.isValid()) 
    date = date_;
  else {
    try { 
      date = conn.last_imported_qdate(cvm);
    } catch ( Dfp::Exception &e ) {
      QVector<QVariant> rootData;
      rootData << tr("Numero") << tr("Conta") << tr("Comentários");
      rootItem = new BalanceItem(rootData);
      return;
    }
  }
//  setupModel();
}
Genet::BalanceModel::~BalanceModel()
{
  delete rootItem;
}

QModelIndex Genet::BalanceModel::index(int row, int column, 
    const QModelIndex &parent) const 
{
  if (parent.isValid() && parent.column() != 0)
        return QModelIndex();
    BalanceItem *parentItem = getItem(parent);

    BalanceItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

Genet::BalanceItem *Genet::BalanceModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        BalanceItem *item = static_cast<BalanceItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

QModelIndex Genet::BalanceModel::parent(const QModelIndex &index) const 
{
  if (!index.isValid())
        return QModelIndex();

    BalanceItem *childItem = getItem(index);
    BalanceItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

QVariant Genet::BalanceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    BalanceItem *item = getItem(index);

    return item->data(index.column());
}

QVariant Genet::BalanceModel::headerData(int section, 
    Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return rootItem->data(section);

  return QVariant();
}

Qt::ItemFlags Genet::BalanceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    if ( (index.column() < 2) | (index.column() > 5)) 
      return QAbstractItemModel::flags(index);
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}
bool Genet::BalanceModel::setData(const QModelIndex &index, 
    const QVariant &value, int role)
{
  if (role != Qt::EditRole)
    return false;

  BalanceItem *item = getItem(index);
  bool result = item->setData(index.column(), value);
  QModelIndex numberIndex = index.sibling(index.row(), 0);
  auto number = numberIndex.data().toString();
  QString date_str;
  if (index.column()<5)
    date_str = rootItem->data(index.column()).toString();
  else if (index.column()==5)
    date_str = rootItem->data(2).toString();

  conn.setValue(cvm, number, date_str, type, index.data());

  if (result)
    emit dataChanged(index, index);

  return result;
}

int Genet::BalanceModel::columnCount(const QModelIndex &parent) const 
{
  return rootItem->columnCount();
}

int Genet::BalanceModel::rowCount(const QModelIndex &parent) const 
{
  BalanceItem *parentItem = getItem(parent);
  return parentItem->childCount();
}

void Genet::BalanceModel::setCvm(int cvm_)
{
  cvm = cvm_;
  setupModel();
}

void Genet::BalanceModel::setExercise(QDate date_)
{
  date = date_;
  setupModel();
}

void Genet::BalanceModel::setBalanceType(Dfp::BalanceType balanceType_)
{
  balanceType = balanceType_;
  setupModel();
}

void Genet::BalanceModel::setType(Dfp::FinancialInfoType type_)
{
  type = type_;
  setupModel();
}

void Genet::BalanceModel::setAnual(bool anual_)
{
  anual = anual_;
  setupModel();
}

