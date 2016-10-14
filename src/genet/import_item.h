/*  import_item.h 
 * 
 *  This file is part of denet. 
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
#ifndef IMPORT_ITEM_INCLUDED  
#define IMPORT_ITEM_INCLUDED
#include <QVector>
#include <QVariant>

namespace Genet {

  class ImportItem
  {
    public:
      explicit ImportItem(const QVector<QVariant> &data, 
          ImportItem *parent = 0);
      ~ImportItem();

      ImportItem *child(int number);
      int childCount() const;
      int columnCount() const;
      QVariant data(int column) const;
      ImportItem *parent();
      bool insertChild(const QVector<QVariant> &data);
      int childNumber() const;
      bool setData(int column, const QVariant &value);


    private:
      ImportItem *parentItem;
      QList<ImportItem*> childItems;
      QVector<QVariant> itemData;
  };
}
#endif
