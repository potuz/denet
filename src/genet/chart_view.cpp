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
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include "chart_view.h"

Q_DECLARE_METATYPE(Dfp::Indicator)

Genet::ChartView::ChartView(int cvm, const GenetDatabase &conn, 
    Dfp::FinancialInfoType type, bool anual, QWidget *parent):
  QWidget(parent), cvm(cvm), conn(conn), type(type), anual(anual)
{
  group = new QGroupBox(tr("Indicadores"));

  QBarSet* setEarnings = new QBarSet(tr("Lucro Líquido"));
  QBarSet* setReceitas = new QBarSet(tr("Receitas"));
  QBarSet* setInterest = new QBarSet(tr("Resultado Financeiro"));
  QBarSet* setPriceEarnings = new QBarSet(tr("Preço / Lucro"));
  QBarSet* setRoe = new QBarSet(tr("ROE"));
  QBarSet* setPvp = new QBarSet(tr("Preço / Valor Patrimonial"));
  QBarSet* setNetEquity = new QBarSet (tr("Patrimônio Líquido"));
  QBarSet* setLiabilities = new QBarSet (tr("Dívida Bruta"));
  QBarSet* setNetLiability = new QBarSet (tr("Dívida Líquida"));
  QBarSet* setLiabilitiesEquity = new QBarSet(
      tr("Dívida Bruta / Patrimônio Líquido"));
  QBarSet* setMargemBruta = new QBarSet(tr("Margem Bruta"));
  QBarSet* setMargemLiquida = new QBarSet(tr("Margem Líquida"));

  auto *earnings = new QCheckBox(tr("&Lucro Líquido"));
  connect(earnings, &QCheckBox::stateChanged, [=](int state) {
      this->addRemoveSeries(state, setEarnings, "3.11");});

  auto *receitas = new QCheckBox(tr("&Receita Líquida"));
  connect(receitas, &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, setReceitas, "3.01");});
  auto *interest = new QCheckBox(tr("Resultado &Financeiro"));
  connect(interest, &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, setInterest, "3.06");});

  auto *priceEarnings = new QCheckBox(tr("&Preço/Lucro"));
  connect(priceEarnings, &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, setPriceEarnings, Dfp::DFP_INDICATOR_PE);});
      
  auto *roe = new QCheckBox(tr("ROE"));
  connect(roe, &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, setRoe, Dfp::DFP_INDICATOR_ROE);});
  auto *pvp = new QCheckBox(tr("Preço/Patrimônio"));
  connect(pvp, &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, setPvp, Dfp::DFP_INDICATOR_PVP);});
  auto *netEquity = new QCheckBox(tr("Patrimônio Líquido"));
  connect(netEquity, &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, setNetEquity, Dfp::DFP_INDICATOR_PL);});
  auto *liabilities = new QCheckBox(tr("&Dívida Bruta"));
  connect(liabilities, &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, setLiabilities, Dfp::DFP_INDICATOR_LIAB);});
  auto *liabilitiesEquity = new QCheckBox(tr("Dívida/Patrimônio"));
  connect(liabilitiesEquity, &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, setLiabilitiesEquity,
          Dfp::DFP_INDICATOR_DIVBRPL);});
  auto *netLiability = new QCheckBox(tr("Dívida Líquida"));
  connect(netLiability, &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, setNetLiability, Dfp::DFP_INDICATOR_NETLIAB);});
  auto *margemBruta = new QCheckBox(tr("Margem &Bruta"));
  connect(margemBruta, &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, setMargemBruta, Dfp::DFP_INDICATOR_MB);});
  auto *margemLiquida = new QCheckBox(tr("Margem Líquida"));
  connect(margemLiquida, &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, setMargemLiquida, Dfp::DFP_INDICATOR_ML);});

  auto groupHLayout = new QHBoxLayout;
  auto groupV1 = new QVBoxLayout; 
  auto groupV2 = new QVBoxLayout; 
  auto groupV3 = new QVBoxLayout; 
  auto groupV4 = new QVBoxLayout; 
  groupHLayout->addLayout(groupV1);
  groupHLayout->addLayout(groupV2);
  groupHLayout->addLayout(groupV3);
  groupHLayout->addLayout(groupV4);
  groupV1->addWidget(earnings);
  groupV1->addWidget(interest);
  groupV1->addWidget(receitas);
  groupV3->addWidget(priceEarnings);
  groupV3->addWidget(roe);
  groupV3->addWidget(pvp);

  groupV2->addWidget(netEquity);
  groupV2->addWidget(liabilities);
  groupV2->addWidget(netLiability);
  groupV4->addWidget(liabilitiesEquity);
  groupV4->addWidget(margemBruta);
  groupV4->addWidget(margemLiquida);
  group->setLayout(groupHLayout);

  barSeries = new QBarSeries();
  chart = new QChart();
  chart->addSeries(barSeries);

  xaxis = new QBarCategoryAxis;
  chart->setAxisX(xaxis, barSeries);
  chart->setTitle(tr("Evolução dos indicadores fundamentalistas."));
  chart->setAnimationOptions(QChart::SeriesAnimations);
  chart->legend()->setVisible(true);

  auto qchartView = new QChartView(chart);
  qchartView->setRenderHint(QPainter::Antialiasing);


  auto layout = new QVBoxLayout;
  layout->addWidget(group);
  layout->addWidget(qchartView);
  setLayout(layout);
}

template <typename Ind>
void Genet::ChartView::addRemoveSeries(int state, QBarSet *set, 
    const Ind& ind) 
{
  if (state == Qt::Checked)
  { 
    set->remove(0,set->count());
    set->append(conn.getSeries(cvm, anual, type, ind));
    qDebug() << "Added " << set->count() << " values with sum" << set->sum() << " to a QbarSet.\n";
    barSeries->append(set);
    qDebug() << "barSeries has " << barSeries->count() << " sets.\n";
    barSeries->detachAxis(xaxis);
    chart->removeAxis(xaxis);
    chart->removeSeries(barSeries);
    chart->addSeries(barSeries);
    chart->createDefaultAxes();
    chart->setAxisX(xaxis, barSeries);
  }
  if (state == Qt::Unchecked)
  {
    barSeries->detachAxis(xaxis);
    chart->removeAxis(xaxis);
    chart->removeSeries(barSeries);
    barSeries->take(set);
    chart->addSeries(barSeries);
    chart->createDefaultAxes();
    chart->setAxisX(xaxis, barSeries);
  }
}

void Genet::ChartView::setAnual(bool anual_)
{
  anual = anual_;
  make_emit();
}

void Genet::ChartView::setCvm(int cvm_)
{
  cvm = cvm_;
  QStringList mylist = conn.get_exercise_list(cvm);
  std::reverse(mylist.begin(), mylist.end());
  xaxis->setCategories(mylist);
  make_emit();
}

void Genet::ChartView::setType(Dfp::FinancialInfoType type_)
{
  type = type_;
  make_emit();
}

void Genet::ChartView::make_emit() const
{
  foreach (auto *p, findChildren<QCheckBox*>())
  {
    QMetaObject::invokeMethod(p, "stateChanged", Q_ARG(int, p->checkState()));
  }
}
