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

namespace {
  const std::array<QString,Genet::ChartView::NUM_INDICATORS> indicatorList {
    QT_TR_NOOP("Lucro Líquido"),
    QT_TR_NOOP("Receitas"),
    QT_TR_NOOP("Resultado Financeiro"),
    QT_TR_NOOP("Preço / Lucro"),
    QT_TR_NOOP("ROE"),
    QT_TR_NOOP("Preço / Valor Patrimonial"),
    QT_TR_NOOP("Patrimônio Líquido"),
    QT_TR_NOOP("Dívida Bruta"),
    QT_TR_NOOP("Dívida Líquida"),
    QT_TR_NOOP("Dívida Bruta / Patrimônio Líquido"),
    QT_TR_NOOP("Margem Bruta"),
    QT_TR_NOOP("Margem Líquida")};

  const std::array<QString,Genet::ChartView::NUM_INDICATORS> checkBoxList {
    QT_TR_NOOP("&Lucro Líquido"),
    QT_TR_NOOP("&Receita Líquida"),
    QT_TR_NOOP("Resultado &Financeiro"),
    QT_TR_NOOP("&Preço/Lucro"),
    QT_TR_NOOP("&ROE"),
    QT_TR_NOOP("Preço/Patrimônio"),
    QT_TR_NOOP("Patrimônio Líquido"),
    QT_TR_NOOP("&Dívida Bruta"),
    QT_TR_NOOP("Dívida Líquida"),
    QT_TR_NOOP("Dívida/Patrimônio"),
    QT_TR_NOOP("Margem &Bruta"),
    QT_TR_NOOP("Margem Líquida")};

  const std::array<const std::string, 3> numInd {"3.11", "3.01", "3.06"};

  const std::array<Dfp::Indicator, Genet::ChartView::NUM_INDICATORS-3> dfpInd {
    Dfp::DFP_INDICATOR_PE, Dfp::DFP_INDICATOR_ROE,
      Dfp::DFP_INDICATOR_PVP, Dfp::DFP_INDICATOR_PL, Dfp::DFP_INDICATOR_LIAB,
      Dfp::DFP_INDICATOR_NETLIAB, Dfp::DFP_INDICATOR_DIVBRPL, 
      Dfp::DFP_INDICATOR_MB, Dfp::DFP_INDICATOR_ML};
}

Genet::ChartView::ChartView(int cvm, const GenetDatabase &conn, 
    Dfp::FinancialInfoType type, bool anual, QWidget *parent):
  QWidget(parent), cvm(cvm), conn(conn), type(type), anual(anual)
{
  QGroupBox* group = new QGroupBox(tr("Indicadores"));
  for (unsigned int i = 0; i < NUM_INDICATORS; i++)
  {
    barSet[i] = new QBarSet (indicatorList[i]);
    checkBox[i] =  new QCheckBox(checkBoxList[i]);
    if (i < 3) 
    connect(checkBox[i], &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, barSet[i], numInd[i]);});
    else 
    connect(checkBox[i], &QCheckBox::stateChanged, [=](int state) {
      addRemoveSeries(state, barSet[i], dfpInd[i-3]);});
  }

  auto groupHLayout = new QHBoxLayout;
  std::array<QVBoxLayout*, 4> groupV; 
  for (int i = 0; i < 4 ; i++)
  {
    groupV[i] = new QVBoxLayout;
    groupHLayout->addLayout(groupV[i]);
    for (int j = 0; j < 3; j++)
      groupV[i]->addWidget(checkBox[3*i+j]);
  }
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
    barSeries->detachAxis(xaxis);
    chart->removeAxis(xaxis);
    chart->removeSeries(barSeries);
    qDebug() << "barSeries has " << barSeries->count() << " sets.";
    qDebug() << "About to remove " << set->count() << " items";
    set->remove(0,set->count());
    set->append(conn.getSeries(cvm, anual, type, ind));
    qDebug() << "Added " << set->count() << " values with sum" << set->sum() << " to a QbarSet.";
    barSeries->append(set);
    qDebug() << "barSeries has " << barSeries->count() << " sets.\n";
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

  barSeries->detachAxis(xaxis);
  chart->removeAxis(xaxis);
  xaxis->setCategories(mylist);
  xaxis->setLabelsAngle(80);
  chart->setAxisX(xaxis, barSeries);
  make_emit();
}

void Genet::ChartView::setType(Dfp::FinancialInfoType type_)
{
  type = type_;
  make_emit();
}

void Genet::ChartView::make_emit()
{
  for (int i = 0; i < NUM_INDICATORS; i++)
  {
    if (i < 3) 
      addRemoveSeries (checkBox[i]->checkState(), barSet[i], numInd[i]);
    else 
      addRemoveSeries (checkBox[i]->checkState(), barSet[i], dfpInd[i-3]);
  }
}
