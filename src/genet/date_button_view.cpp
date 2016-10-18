/*  date_button_view.cpp 
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
#include "date_button_view.h"
#include "genet_database.h"
#include <QStringListModel>

Genet::DateButtonView::DateButtonView(int cvm, const GenetDatabase &conn, 
    QWidget *parent) : QWidget(parent), cvm(cvm), conn(conn)
{
  button = new QComboBox;
  model = new QStringListModel;
//  setupModel();
  button->setModel(model);
  button->setSizeAdjustPolicy(QComboBox::AdjustToContents);

  connect(button, SIGNAL(currentIndexChanged(const QString&)), this,
      SLOT(emitDate(const QString&)) );
  auto layout = new QVBoxLayout;
  layout->addWidget(button);
  setLayout(layout);
}

void Genet::DateButtonView::setCvm(int cvm_)
{
  cvm = cvm_;
  setupModel();
  button->setCurrentIndex(0);
}

void Genet::DateButtonView::emitDate(const QString &date_str)
{
  QDate date = QDate::fromString(date_str, "yyyy-MM-dd");
  emit changedDate(date);
}

void Genet::DateButtonView::setupModel()
{
  model->setStringList(conn.get_exercise_list(cvm));
}

void Genet::DateButtonView::setCurrentIndex(int index)
{
  button->setCurrentIndex(index);
}
