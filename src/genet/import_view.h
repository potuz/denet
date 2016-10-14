/*  import_view.h 
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
#ifndef IMPORT_VIEW_INCLUDED
#define IMPORT_VIEW_INCLUDED
#include <QDialog>
#include <QItemSelectionModel>
#include <QTreeView>

namespace Genet { 
  class GenetDatabase;

  class ImportView : public QDialog 
  {
    Q_OBJECT

    public: 
      ImportView(int cvm, const GenetDatabase &conn, QWidget *parent=0);

    protected slots:
      void processSelection(const QItemSelection& selected, 
                                      const QItemSelection& deselected) const;
      void accept();
    private:
      QTreeView *view;
      const GenetDatabase &conn;

  };
}

#endif
