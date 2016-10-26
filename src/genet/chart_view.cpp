/*  chart_view.cpp 
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
#include "chart_view.h"

Genet::ChartView::ChartView(int cvm, const GenetDatabase &conn, 
    Dfp::FinancialInfoType type, bool anual, QWidget *parent):
  QWidget(parent), cvm(cvm), type(type), anual(anual), conn(conn)
{
  group = new QGroupBox(tr("Indicadores"));

  auto *earnings = new QCheckBox(tr("&Lucro Líquido"));
  auto *receitas = new QCheckBox(tr("&Receita Líquida"));
  auto *interest = new QCheckBox(tr("Resultado &Financeiro"));

  auto *priceEarnings = new QCheckBox(tr("&Preço/Lucro"));
  auto *roe = new QCheckBox(tr("ROE"));
  auto *pvp = new QCheckBox(tr("Preço/Patrimônio"));

  auto *netEquity = new QCheckBox(tr("Patrimônio Líquido"));
  auto *liabilities = new QCheckBox(tr("&Dívida Bruta"));
  auto *liabilitiesEquity = new QCheckBox(tr("Dívida/Patrimônio"));

  auto *netLiability = new QCheckBox(tr("Dívida Líquida"));
  auto *margemBruta = new QCheckBox(tr("Margem &Bruta"));
  auto *margemLiquida = neq QCheckBox(tr("Margem Líquida"));

  auto groupHLayout = new QHBoxLayout;
  auto groupV1 = new QVBoxLayout; 
  auto groupV2 = new QVBoxLayout; 
  groupHLayout->addWidget(groupV1);
  groupHLayout->addWidget(groupV2);
  groupV1->addWidget(earnings);
  groupV1->addWidget(interest);
  groupV1->addWidget(receitas);
  groupV1->addWidget(priceEarnings);
  groupV1->addWidget(roe);
  groupV1->addWidget(pvp);

  groupV2->addWidget(netEquity);
  groupV2->addWidget(liabilities);
  groupV2->addWidget(netLiability);
  groupV2->addWidget(liabilitiesEquity);
  groupV2->addWidget(margemBruta);
  groupV2->addWidget(margemLiquida);
  group->setLayout(groupHLayout);
  group->setMaximumSize(400,400);



  auto layout = new QVBoxLaoyout;
  layout->addWidget(chart);
  setLayout(layout);
}

void Genet::ChartView::setAnual(void)
{
}

void Genet::ChartView::setCvm(int)
{
}

void Genet::ChartView::setType(Dfp::DfpFinancialInfoType)
{
}

