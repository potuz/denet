/*  import_progress_dialog.h 
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
/// \file
#ifndef IMPORT_PROGRESS_DIALOG_H
#define IMPORT_PROGRESS_DIALOG_H
#include <QProgressDialog>

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE


namespace Genet {
  class ImportProgressDialog : public  QProgressDialog
  {
    Q_OBJECT
    public: 
      ImportProgressDialog(QWidget *parent = 0);
    public slots: 
      void appendText (const QString &str);
    private:
      QTextEdit *textEdit;
};
}

#endif
