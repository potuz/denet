/*  import_item.cpp 
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
#include "import_item.h"

Genet::ImportItem::ImportItem(const QVector<QVariant> &data, 
    ImportItem *parent) : parentItem (parent), itemData(data)
{
}

Genet::ImportItem::~ImportItem()
{
  qDeleteAll(childItems);
}

Genet::ImportItem *Genet::ImportItem::child(int number)
{
  return childItems.value(number);
}

Genet::ImportItem *Genet::ImportItem::parent()
{
  return parentItem;
}

QVariant Genet::ImportItem::data(int column) const
{
  return itemData.value(column);
}

bool Genet::ImportItem::setData(int column, const QVariant &value)
{
  if (column < 0 || column >= itemData.size())
    return false;

  itemData[column] = value;
  return true;
}

int Genet::ImportItem::childCount() const
{
  return childItems.count();
}

int Genet::ImportItem::childNumber() const
{
  if (parentItem)
    return parentItem->childItems.indexOf(const_cast<ImportItem*>(this));

  return 0;
}

int Genet::ImportItem::columnCount() const
{
  return itemData.count();
}

bool Genet::ImportItem::insertChild(const QVector<QVariant> &data)
{
  Genet::ImportItem *newItem = new Genet::ImportItem (data, this);
  childItems.append (newItem);
  return true;
}


