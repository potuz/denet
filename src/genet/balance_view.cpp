/*  balance_view.cpp 
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
#include "balance_view.h"
#include "balance_model.h"

Genet::BalanceView::BalanceView(int cvm, 
    const GenetDatabase &conn, 
    Dfp::BalanceType balanceType, 
    bool anual,
    QDate date, 
    Dfp::FinancialInfoType type, 
    QWidget *parent) 
{
  view = new QTreeView;
  auto *model = new BalanceModel (cvm, conn, balanceType, anual, date, type,
      parent);
  view->setModel(model);
  connect(this, SIGNAL(changedCvm(int)), model, SLOT(setCvm(int)));
  connect(this, SIGNAL(changedDate(QDate)), model, SLOT(setExercise(QDate)));
  connect(this, SIGNAL(changedType(Dfp::FinancialInfoType)), model,
      SLOT(setType(Dfp::FinancialInfoType)));
  connect(this, SIGNAL(changedBalanceType(Dfp::BalanceType)), model,
      SLOT(setBalanceType(Dfp::BalanceType)));
  connect(this, SIGNAL(changedAnual(bool)), model,
      SLOT(setAnual(bool)));
  view->expandAll();
  view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  view->setAlternatingRowColors(true);
  auto layout = new QVBoxLayout;
  layout->addWidget(view);
  setLayout(layout);
}

void Genet::BalanceView::setAnual(bool anual_)
{
  emit changedAnual(anual_);
}

void Genet::BalanceView::setBalanceType(Dfp::BalanceType type_)
{
  emit changedBalanceType(type_);
}

void Genet::BalanceView::setCvm(int cvm_)
{
  emit changedCvm(cvm_);
}

void Genet::BalanceView::setDate(QDate date_)
{
  emit changedDate(date_);
}

void Genet::BalanceView::setType(Dfp::FinancialInfoType type_)
{
  emit changedType(type_);
}



