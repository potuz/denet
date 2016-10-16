/*  import_progress_dialog.cpp 
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
#include "import_progress_dialog.h"
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QLayout>

Genet::ImportProgressDialog::ImportProgressDialog(QWidget *parent) : 
  QProgressDialog(parent)
{
  setWindowModality(Qt::WindowModal);
  textEdit = new QTextEdit;
  textEdit->setReadOnly(true);

  auto label = new QLabel(tr("Importando arquivos para a base de dados..."));
  auto bar = new QProgressBar;
  auto cancelButton = new QPushButton (tr("Cancelar"));

  setBar(bar);
  setLabel(label);
  setCancelButton(cancelButton);

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(label, 0,0,1,2);
  layout->addWidget(bar,1,0,1,2);
  layout->addWidget(textEdit,2,0,1,2);
  layout->addWidget(cancelButton, 3,1,1,1);
  setLayout(layout);
}

void Genet::ImportProgressDialog::appendText(const QString &str)
{
  textEdit->append(str);
}

