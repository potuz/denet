/*  import_model.h 
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
#ifndef IMPORT_MODEL_INCLUDED
#define IMPORT_MODEL_INCLUDED
/// \file
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "import_item.h"

namespace Genet {

  class GenetDatabase;

  class ImportModel : public QAbstractItemModel
  {
    Q_OBJECT

    public:
      ImportModel(int cvm, const GenetDatabase &conn, QObject *parent = 0);
      ~ImportModel();

      QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
      QVariant headerData(int section, Qt::Orientation orientation,
          int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

      QModelIndex index(int row, int column,
          const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
      QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
      ImportItem *getItem(const QModelIndex &index) const;

      int rowCount(
          const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
      int columnCount(
          const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    private:
      ImportItem *rootItem;
      int cvm;
      const GenetDatabase &conn;
  };
}
#endif
