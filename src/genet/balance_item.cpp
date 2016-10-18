/*  balance_item.cpp 
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
#include "balance_item.h"

Genet::BalanceItem *Genet::BalanceItem::child(int number)
{
  return childItems.value(number);
}

Genet::BalanceItem *Genet::BalanceItem::parent()
{
  return parentItem;
}

Genet::BalanceItem::BalanceItem(const QVector<QVariant> &data, 
    BalanceItem *parent) : parentItem (parent), itemData(data)
{
}

Genet::BalanceItem::~BalanceItem()
{
  qDeleteAll(childItems);
}

QVariant Genet::BalanceItem::data(int column) const
{
  return itemData.value(column);
}

Genet::BalanceItem *Genet::BalanceItem::insertChild(const QVector<QVariant> &data)
{
  Genet::BalanceItem *newItem = new Genet::BalanceItem (data, this);
  childItems.append (newItem);
  return newItem;
}

int Genet::BalanceItem::childCount() const
{
  return childItems.count();
}

int Genet::BalanceItem::childNumber() const
{
  if (parentItem)
    return parentItem->childItems.indexOf(const_cast<BalanceItem*>(this));

  return 0;
}

int Genet::BalanceItem::columnCount() const
{
  return itemData.count();
}

bool Genet::BalanceItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}

