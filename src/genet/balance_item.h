/*  balance_item.h 
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
#ifndef BALANCE_ITEM_H
#define BALANCE_ITEM_H
/// \file
#include <QVariant>
#include <QVector>

namespace Genet {
  class BalanceItem
  {
    public:
      explicit BalanceItem (const QVector<QVariant> &data, 
          BalanceItem *parent=0);
      ~BalanceItem();

      BalanceItem *child(int number);
      int childCount() const;
      int columnCount() const;
      QVariant data(int column) const;
      BalanceItem *insertChild(const QVector<QVariant> &data);
      BalanceItem *parent();
      int childNumber() const;
      bool setData(int column, const QVariant &value);

    private:
      QList<BalanceItem*> childItems;
      BalanceItem *parentItem;
      QVector<QVariant> itemData;
  };
}

#endif
