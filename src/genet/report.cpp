/*  report.cpp 
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
#include "report.h"
#include "dfp/dfp_company.h"
#include <QDebug>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>

QT_CHARTS_USE_NAMESPACE
namespace{
  template<typename T>
    QString coltd(T number) 
    {
      if (number < 0)
        return QString("<font color=\"#ff0000\">%L1</color>").arg(number);
      else 
        return QString("<font color=\"#000000\">%L1</color>").arg(number);
    }
  template<typename T>
    QString colperc(T number) 
    {
      if (number < 0)
        return QString("<font color=\"#ff0000\">%L1</color>").
          arg(number,0,'f', 2);
      else 
        return QString("<font color=\"#000000\">%L1</color>").
          arg(number,0,'f', 2);
    }
}

Genet::Report::Report(const GenetDatabase& conn, int cvm, 
    Dfp::FinancialInfoType type) :
  Report (conn, cvm, type, conn.last_imported_qdate(cvm))
{
}

Genet::Report::Report(const GenetDatabase& conn, int cvm, 
    Dfp::FinancialInfoType type, const QDate& date) : conn(conn),
  cvm(cvm),type(type),date(date) 
{
  createPerformanceChart();
  createEquityChart();
  createHtml();
  html.append("<p><p><div align=\"center\"><font size=\"1\">"
    "processed by denet (C) 2016 Potuz &lt;potuz@potuz.net&gt;</font></div>");
}

void Genet::Report::createHtml()
{
  Dfp::Company company (conn.get_company_from_cvm(cvm));
  //1st Heading
  html = QString(
      "<html>"
      "<head>"
      "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
      "</head>"
      "<body>"
      "<div align=\"center\"><h1>%1 Resultados de %2</h1></div>").\
    arg(date.toString("yyyy-MM-dd")).arg(company.getName().c_str());

  QDate previousItr = date.addMonths(-3);
  previousItr = QDate(previousItr.year(), previousItr.month(), 
      previousItr.daysInMonth());
  QDate previousYear = date.addYears(-1);

  //Results table
  html.append(QString(
        "<div align=\"center\">"
        "<h2>Demonstrativos de Resultados</h2>"
        "<p>"
        "<table border=\"1\" border-color=\"black\" border-style=\"solid\" "
        "cellspacing=\"0\">"
        "<tr bgcolor=\"#e9efcf\">"
        "<th>Conta</th>"
        "<th>Valor em %1</th>"
        "<th>Acumulado 12m</th>"
        "<th>Valor em %2</th>"
        "<th>Variação</th>"
        "<th>Valor em %3</th>"
        "<th>Variação</th>"
        "</tr>").arg(date.toString("yyyy-MM-dd")).
      arg(previousItr.toString("yyyy-MM-dd")).arg(previousYear.
        toString("yyyy-MM-dd")));

  int retItr = conn.get_indicator(cvm, "3.01", date,false, type);
  int retYear = conn.get_indicator(cvm, "3.01", date, true, type);
  int retPreviousItr = conn.get_indicator(cvm, "3.01", previousItr, false,
      type);
  int retItrLastYear = conn.get_indicator(cvm, "3.01", previousYear, false, 
      type);
  html.append(QString(
        "<tr bgcolor=\"#d0e8ce\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2</td>"
        "<td align=\"right\">%L3</td>"
        "<td align=\"right\">%L4</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Receitas Líquidas").
      arg(coltd(retItr)).
      arg(coltd(retYear)).
      arg(coltd(retPreviousItr)).
      arg((retPreviousItr) ? 
        (float) (retItr-retPreviousItr)/retPreviousItr*100 :
        (float) 0,
        0, 'f', 2).
      arg(coltd(retItrLastYear)).
      arg((retItrLastYear) ? 
        (float) (retItr-retItrLastYear)/retItrLastYear*100 : 
        0.0,
        0, 'f', 2));

  retItr = conn.get_indicator(cvm, "3.02", date,false, type);
  retYear = conn.get_indicator(cvm, "3.02", date, true, type);
  retPreviousItr = conn.get_indicator(cvm, "3.02", previousItr, false, type);
  retItrLastYear = conn.get_indicator(cvm, "3.02", previousYear, false, type);
  html.append(QString(
        "<tr bgcolor=\"#e9efcf\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2</td>"
        "<td align=\"right\">%L3</td>"
        "<td align=\"right\">%L4</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Custos dos produtos").
      arg(coltd(retItr)).
      arg(coltd(retYear)).
      arg(coltd(retPreviousItr)).
      arg((retPreviousItr) ? (float) (retItr-retPreviousItr)/retPreviousItr*100 : 0,
        0, 'f', 2).
      arg(coltd(retItrLastYear)).
      arg((retItrLastYear) ? (float) (retItr-retItrLastYear)/retItrLastYear*100 : 0,
        0, 'f', 2));

  retItr = conn.get_indicator(cvm, "3.03", date,false, type);
  retYear = conn.get_indicator(cvm, "3.03", date, true, type);
  retPreviousItr = conn.get_indicator(cvm, "3.03", previousItr, false, type);
  retItrLastYear = conn.get_indicator(cvm, "3.03", previousYear, false, type);
  html.append(QString(
        "<tr bgcolor=\"#d0e8ce\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2</td>"
        "<td align=\"right\">%L3</td>"
        "<td align=\"right\">%L4</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Resultado Bruto").
      arg(coltd(retItr)).
      arg(coltd(retYear)).
      arg(coltd(retPreviousItr)).
      arg((retPreviousItr) ? (float) (retItr-retPreviousItr)/retPreviousItr*100 : 0,
        0, 'f', 2).
      arg(coltd(retItrLastYear)).
      arg((retItrLastYear) ? (float) (retItr-retItrLastYear)/retItrLastYear*100 : 0,
        0, 'f', 2));
  
  float floatRetItr = conn.get_indicator(cvm, Dfp::DFP_INDICATOR_MB, date,
      false, type);
  float floatRetYear = conn.get_indicator(cvm, Dfp::DFP_INDICATOR_MB, date, 
      true, type);
  float floatRetPreviousItr = conn.get_indicator(cvm, Dfp::DFP_INDICATOR_MB,
      previousItr, false, type);
  float floatRetItrLastYear = conn.get_indicator(cvm, Dfp::DFP_INDICATOR_MB,
      previousYear, false, type);
  html.append(QString(
        "<tr bgcolor=\"#e9efcf\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2\%</td>"
        "<td align=\"right\">%L3\%</td>"
        "<td align=\"right\">%L4\%</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6\%</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Margem Bruta").
      arg(colperc(floatRetItr*100)).
      arg(colperc(floatRetYear*100)).
      arg(colperc(floatRetPreviousItr*100)).
      arg((floatRetPreviousItr) ?
        (float) (floatRetItr-floatRetPreviousItr)/floatRetPreviousItr*100 : 0,
        0, 'f', 2).
      arg(colperc(floatRetItrLastYear*100)).
      arg((floatRetItrLastYear) ? 
        (float) (floatRetItr-floatRetItrLastYear)/floatRetItrLastYear*100 : 0,
        0, 'f', 2));

  retItr = conn.get_indicator(cvm, "3.05", date,false, type);
  retYear = conn.get_indicator(cvm, "3.05", date, true, type);
  retPreviousItr = conn.get_indicator(cvm, "3.05", previousItr, false, type);
  retItrLastYear = conn.get_indicator(cvm, "3.05", previousYear, false, type);
  html.append(QString(
        "<tr bgcolor=\"#d0e8ce\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2</td>"
        "<td align=\"right\">%L3</td>"
        "<td align=\"right\">%L4</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("EBIT").
      arg(coltd(retItr)).
      arg(coltd(retYear)).
      arg(coltd(retPreviousItr)).
      arg((retPreviousItr) ? (float) (retItr-retPreviousItr)/retPreviousItr*100 : 0,
        0, 'f', 2).
      arg(coltd(retItrLastYear)).
      arg((retItrLastYear) ? (float) (retItr-retItrLastYear)/retItrLastYear*100 : 0,
        0, 'f', 2));
  
  floatRetItr = conn.get_indicator(cvm, Dfp::DFP_INDICATOR_MEBIT, date,
      false, type);
  floatRetYear = conn.get_indicator(cvm, Dfp::DFP_INDICATOR_MEBIT, date, 
      true, type);
  floatRetPreviousItr = conn.get_indicator(cvm, Dfp::DFP_INDICATOR_MEBIT,
      previousItr, false, type);
  floatRetItrLastYear = conn.get_indicator(cvm, Dfp::DFP_INDICATOR_MEBIT,
      previousYear, false, type);
  html.append(QString(
        "<tr bgcolor=\"#e9efcf\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2\%</td>"
        "<td align=\"right\">%L3\%</td>"
        "<td align=\"right\">%L4\%</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6\%</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Margem EBIT").
      arg(colperc(floatRetItr*100)).
      arg(colperc(floatRetYear*100)).
      arg(colperc(floatRetPreviousItr*100)).
      arg((floatRetPreviousItr) ?
        (float) (floatRetItr-floatRetPreviousItr)/floatRetPreviousItr*100 : 0,
        0, 'f', 2).
      arg(colperc(floatRetItrLastYear*100)).
      arg((floatRetItrLastYear) ? 
        (float) (floatRetItr-floatRetItrLastYear)/floatRetItrLastYear*100 : 0,
        0, 'f', 2));

  retItr = conn.get_indicator(cvm, "3.06", date,false, type);
  retYear = conn.get_indicator(cvm, "3.06", date, true, type);
  retPreviousItr = conn.get_indicator(cvm, "3.06", previousItr, false, type);
  retItrLastYear = conn.get_indicator(cvm, "3.06", previousYear, false, type);
  html.append(QString(
        "<tr bgcolor=\"#d0e8ce\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2</td>"
        "<td align=\"right\">%L3</td>"
        "<td align=\"right\">%L4</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Resultado Financeiro").
      arg(coltd(retItr)).
      arg(coltd(retYear)).
      arg(coltd(retPreviousItr)).
      arg((retPreviousItr) ? (float) (retItr-retPreviousItr)/retPreviousItr*100 : 0,
        0, 'f', 2).
      arg(coltd(retItrLastYear)).
      arg((retItrLastYear) ? (float) (retItr-retItrLastYear)/retItrLastYear*100 : 0,
        0, 'f', 2));

  retItr = conn.get_indicator(cvm, "3.11", date,false, type);
  retYear = conn.get_indicator(cvm, "3.11", date, true, type);
  retPreviousItr = conn.get_indicator(cvm, "3.11", previousItr, false, type);
  retItrLastYear = conn.get_indicator(cvm, "3.11", previousYear, false, type);
  html.append(QString(
        "<tr bgcolor=\"#e9efcf\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2</td>"
        "<td align=\"right\">%L3</td>"
        "<td align=\"right\">%L4</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Resultado Líquido").
      arg(coltd(retItr)).
      arg(coltd(retYear)).
      arg(coltd(retPreviousItr)).
      arg((retPreviousItr) ? (float) (retItr-retPreviousItr)/retPreviousItr*100 : 0,
        0, 'f', 2).
      arg(coltd(retItrLastYear)).
      arg((retItrLastYear) ? (float) (retItr-retItrLastYear)/retItrLastYear*100 : 0,
        0, 'f', 2));
 
  floatRetItr = conn.get_indicator(cvm, Dfp::DFP_INDICATOR_ML, date,
      false, type);
  floatRetYear = conn.get_indicator(cvm, Dfp::DFP_INDICATOR_ML, date, 
      true, type);
  floatRetPreviousItr = conn.get_indicator(cvm, Dfp::DFP_INDICATOR_ML,
      previousItr, false, type);
  floatRetItrLastYear = conn.get_indicator(cvm, Dfp::DFP_INDICATOR_ML,
      previousYear, false, type);
  html.append(QString(
        "<tr bgcolor=\"#d0e8ce\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2\%</td>"
        "<td align=\"right\">%L3\%</td>"
        "<td align=\"right\">%L4\%</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6\%</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Margem Líquida").
      arg(colperc(floatRetItr*100)).
      arg(colperc(floatRetYear*100)).
      arg(colperc(floatRetPreviousItr*100)).
      arg((floatRetPreviousItr) ?
        (float) (floatRetItr-floatRetPreviousItr)/floatRetPreviousItr*100 : 0,
        0, 'f', 2).
      arg(colperc(floatRetItrLastYear*100)).
      arg((floatRetItrLastYear) ? 
        (float) (floatRetItr-floatRetItrLastYear)/floatRetItrLastYear*100 : 0,
        0, 'f', 2));

  html.append("</table>"
      "</div>"
      "</p>");
  
  //DFC table
  html.append(QString(
        "<div align=\"center\">"
        "<h2>Demonstrativos de Fluxo de Caixa</h2>"
        "<p>"
        "<table border=\"1\" border-color=\"black\" border-style=\"solid\" "
        "cellspacing=\"0\">"
        "<tr bgcolor=\"#fce5f4\">"
        "<th>Conta</th>"
        "<th>Valor em %1</th>"
        "<th>Acumulado 12m</th>"
        "<th>Valor em %2</th>"
        "<th>Variação</th>"
        "<th>Valor em %3</th>"
        "<th>Variação</th>"
        "</tr>").arg(date.toString("yyyy-MM-dd")).
      arg(previousItr.toString("yyyy-MM-dd")).arg(previousYear.
        toString("yyyy-MM-dd")));

  retItr = conn.get_indicator(cvm, "6.01", date,false, type);
  retYear = conn.get_indicator(cvm, "6.01", date, true, type);
  retPreviousItr = conn.get_indicator(cvm, "6.01", previousItr, false,
      type);
  retItrLastYear = conn.get_indicator(cvm, "6.01", previousYear, false, 
      type);
  html.append(QString(
        "<tr bgcolor=\"#ffddde\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2</td>"
        "<td align=\"right\">%L3</td>"
        "<td align=\"right\">%L4</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Caixa Líquido de Ativ. Oper.").
      arg(coltd(retItr)).
      arg(coltd(retYear)).
      arg(coltd(retPreviousItr)).
      arg((retPreviousItr) ? 
        (float) (retItr-retPreviousItr)/retPreviousItr*100 :
        (float) 0,
        0, 'f', 2).
      arg(coltd(retItrLastYear)).
      arg((retItrLastYear) ? 
        (float) (retItr-retItrLastYear)/retItrLastYear*100 : 
        0.0,
        0, 'f', 2));

  retItr = conn.get_indicator(cvm, "6.02", date,false, type);
  retYear = conn.get_indicator(cvm, "6.02", date, true, type);
  retPreviousItr = conn.get_indicator(cvm, "6.02", previousItr, false,
      type);
  retItrLastYear = conn.get_indicator(cvm, "6.02", previousYear, false, 
      type);
  html.append(QString(
        "<tr bgcolor=\"#fce5f4\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2</td>"
        "<td align=\"right\">%L3</td>"
        "<td align=\"right\">%L4</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Caixa Líquido de Investimento").
      arg(coltd(retItr)).
      arg(coltd(retYear)).
      arg(coltd(retPreviousItr)).
      arg((retPreviousItr) ? 
        (float) (retItr-retPreviousItr)/retPreviousItr*100 :
        (float) 0,
        0, 'f', 2).
      arg(coltd(retItrLastYear)).
      arg((retItrLastYear) ? 
        (float) (retItr-retItrLastYear)/retItrLastYear*100 : 
        0.0,
        0, 'f', 2));

  retItr = conn.get_indicator(cvm, "6.03", date,false, type);
  retYear = conn.get_indicator(cvm, "6.03", date, true, type);
  retPreviousItr = conn.get_indicator(cvm, "6.03", previousItr, false,
      type);
  retItrLastYear = conn.get_indicator(cvm, "6.03", previousYear, false, 
      type);
  html.append(QString(
        "<tr bgcolor=\"#ffddde\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2</td>"
        "<td align=\"right\">%L3</td>"
        "<td align=\"right\">%L4</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Caixa Líquido de Financiamento").
      arg(coltd(retItr)).
      arg(coltd(retYear)).
      arg(coltd(retPreviousItr)).
      arg((retPreviousItr) ? 
        (float) (retItr-retPreviousItr)/retPreviousItr*100 :
        (float) 0,
        0, 'f', 2).
      arg(coltd(retItrLastYear)).
      arg((retItrLastYear) ? 
        (float) (retItr-retItrLastYear)/retItrLastYear*100 : 
        0.0,
        0, 'f', 2));

  retItr = conn.get_indicator(cvm, "6.04", date,false, type);
  retYear = conn.get_indicator(cvm, "6.04", date, true, type);
  retPreviousItr = conn.get_indicator(cvm, "6.04", previousItr, false,
      type);
  retItrLastYear = conn.get_indicator(cvm, "6.04", previousYear, false, 
      type);
  html.append(QString(
        "<tr bgcolor=\"#fce5f4\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2</td>"
        "<td align=\"right\">%L3</td>"
        "<td align=\"right\">%L4</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Variação Cambial").
      arg(coltd(retItr)).
      arg(coltd(retYear)).
      arg(coltd(retPreviousItr)).
      arg((retPreviousItr) ? 
        (float) (retItr-retPreviousItr)/retPreviousItr*100 :
        (float) 0,
        0, 'f', 2).
      arg(coltd(retItrLastYear)).
      arg((retItrLastYear) ? 
        (float) (retItr-retItrLastYear)/retItrLastYear*100 : 
        0.0,
        0, 'f', 2));

  retItr = conn.get_indicator(cvm, "6.05", date,false, type);
  retYear = conn.get_indicator(cvm, "6.05", date, true, type);
  retPreviousItr = conn.get_indicator(cvm, "6.05", previousItr, false,
      type);
  retItrLastYear = conn.get_indicator(cvm, "6.05", previousYear, false, 
      type);
  html.append(QString(
        "<tr bgcolor=\"#ffddde\">"
        "<td>%1</td>"
        "<td align=\"right\">%L2</td>"
        "<td align=\"right\">%L3</td>"
        "<td align=\"right\">%L4</td>"
        "<td align=\"right\">%L5\%</td>"
        "<td align=\"right\">%L6</td>"
        "<td align=\"right\">%L7\%</td>"
        "</tr>").arg("Aumento (redução) de Caixa e Equiv.").
      arg(coltd(retItr)).
      arg(coltd(retYear)).
      arg(coltd(retPreviousItr)).
      arg((retPreviousItr) ? 
        (float) (retItr-retPreviousItr)/retPreviousItr*100 :
        (float) 0,
        0, 'f', 2).
      arg(coltd(retItrLastYear)).
      arg((retItrLastYear) ? 
        (float) (retItr-retItrLastYear)/retItrLastYear*100 : 
        0.0,
        0, 'f', 2));
  html.append("</table>"
      "</div>"
      "<p>");

  //Principal indicators
  html.append(QString(
        "<div align=\"center\">"
        "<h2>Principais Indicadores Fundamentalistas</h2>"
        "<p>"
        "<table border=\"1\" border-color=\"black\" border-style=\"solid\" "
        "cellspacing=\"0\">"
        "<tr bgcolor=\"#e9efcf\">"
        "<th>Indicadores de Preço</th>"
        "<th>Indicadores de Performance</th>"
        "</tr>"
        "<tr bgcolor=\"#d0e8ce\">"
        "<td>P/L: %L1</td>"
        "<td>ROE: %L2\%</td>"
        "</tr>").arg(colperc(conn.get_indicator(cvm,Dfp::DFP_INDICATOR_PE,date,
              true, type))).
      arg(colperc(conn.get_indicator(cvm, Dfp::DFP_INDICATOR_ROE, date, true,
            type)*100)));
  html.append(QString(
        "<tr bgcolor=\"#e9efcf\">"
        "<td>P/EBIT: %L1</td>"
        "<td>ROIC: %L2\%</td>"
        "</tr>").arg(colperc(conn.get_indicator(cvm,Dfp::DFP_INDICATOR_PEBIT,date,
              true, type))).
      arg(colperc(conn.get_indicator(cvm, Dfp::DFP_INDICATOR_ROIC, date, true,
            type)*100)));
  html.append(QString(
        "<tr bgcolor=\"#d0e8ce\">"
        "<td>PSR: %L1</td>"
        "<td>Liq Corrente: %L2</td>"
        "</tr>").arg(colperc(conn.get_indicator(cvm,Dfp::DFP_INDICATOR_PSR,date,
              true, type))).
      arg(colperc(conn.get_indicator(cvm, Dfp::DFP_INDICATOR_LIQCORR, date, true,
            type))));
  html.append(QString(
        "<tr bgcolor=\"#e9efcf\">"
        "<td>EV/EBIT: %L1</td>"
        "<td>Div. Br./ PL: %L2</td>"
        "</tr>").arg(colperc(conn.get_indicator(cvm,Dfp::DFP_INDICATOR_EVEBIT,date,
              true, type))).
      arg(colperc(conn.get_indicator(cvm, Dfp::DFP_INDICATOR_DIVBRPL, date, true,
            type))));
  html.append(QString(
        "<tr bgcolor=\"#d0e8ce\">"
        "<td>P/PL: %L1\%</td>"
        "<td> </td>"
        "</tr>").arg(colperc(conn.get_indicator(cvm,Dfp::DFP_INDICATOR_PVP,date,
              true, type)*100)));
  html.append(QString(
        "<tr bgcolor=\"#e9efcf\">"
        "<td>P/Cap. Giro: %L1</td>"
        "<td> </td>"
        "</tr>").arg(colperc(conn.get_indicator(cvm,Dfp::DFP_INDICATOR_PWORKCAP,
              date,true, type))));
  html.append(QString(
        "<tr bgcolor=\"#d0e8ce\">"
        "<td>P/Ativ. Circ. Líq.: %L1</td>"
        "<td>Div. Yield: %L2\%</td>"
        "</tr>").arg(colperc(conn.get_indicator(cvm,Dfp::DFP_INDICATOR_PNETLIQASSET,
              date,true, type))).
      arg(colperc(conn.get_indicator(cvm, Dfp::DFP_INDICATOR_DY, date, true,
            type)*100)));
  html.append("</table>"
      "</div>"
      "<p>");

  //Add a chart
  html.append(QString(
      "<div align=\"center\">"
      "<p><h2>Graficos de Evolução</h2></p>"
      "<p>"
      "<img src=\"%1\">"
      "</div><p>").arg(chartPerformanceFile));

  //Add an Equity chart
  html.append(QString(
      "<div align=\"center\">"
      "<p>"
      "<img src=\"%1\">"
      "</div>").arg(chartEquityFile));

  //Finish up
  html.append("</body>"
      "</html>");
}

void Genet::Report::createPerformanceChart()
{
  QChart* chart = new QChart();

  chart->setTitle("Evolução dos indicadores de Performance");
  chart->legend()->setVisible(true);

  QStringList mylist = conn.get_exercise_list(cvm);
  std::reverse(mylist.begin(), mylist.end());
  auto xaxis = new QBarCategoryAxis;
  xaxis->setCategories(mylist);
  xaxis->setLabelsAngle(80);
 
  auto barSeries = new QBarSeries();
  auto set = new QBarSet("Lucro");
  set->append(conn.getSeries(cvm, false, type, "3.11"));
  barSeries->append(set);
  set = new QBarSet("EBIT");
  set->append(conn.getSeries(cvm, false ,type, "3.05"));
  barSeries->append(set);
  auto barYAxis = new QValueAxis();
  chart->addSeries(barSeries);
  chart->addAxis(barYAxis, Qt::AlignLeft);
  chart->addAxis(xaxis, Qt::AlignBottom);

  auto lineSeries = new QSplineSeries();
  lineSeries->setName("Margem Bruta");
  QList<qreal> listMB(conn.getSeries(cvm,false, type, Dfp::DFP_INDICATOR_MB));
  for (int i = 0; i< listMB.count(); i++)
    lineSeries->append(i,listMB.at(i));
  chart->addSeries(lineSeries);
  auto lineYAxis  = new QValueAxis();
  chart->addAxis(lineYAxis, Qt::AlignRight);
  lineSeries->attachAxis(lineYAxis);
  lineSeries->attachAxis(xaxis);

  lineSeries = new QSplineSeries();
  lineSeries->setName("Margem EBIT");
  QList<qreal> listEBIT(conn.getSeries(cvm,false, type, 
        Dfp::DFP_INDICATOR_MEBIT));
  for (int i = 0; i< listEBIT.count(); i++)
    lineSeries->append(i,listEBIT.at(i));
  chart->addSeries(lineSeries);

  auto lineYAxis2  = new QValueAxis();
  chart->addAxis(lineYAxis2, Qt::AlignRight);
  lineYAxis2->setVisible(false);
  lineSeries->attachAxis(lineYAxis2);
  lineSeries->attachAxis(xaxis);

  qreal min = qMin(lineYAxis->min(), lineYAxis2->min());
  qreal max = qMax(lineYAxis->max(), lineYAxis2->max());

  lineYAxis->setRange(min,max);
  lineYAxis2->setRange(min,max);

  barSeries->attachAxis(xaxis);
  barSeries->attachAxis(barYAxis);
  auto chartView = new QChartView(chart);
  chartView->resize(1000,600);

  if (!dir.isValid()) throw std::runtime_error ("Can't create dir\n");
  chartPerformanceFile = dir.path();
  #ifdef _WIN32  
  chartPerformanceFile.append("\\");
  #elif defined (__linux)
  chartPerformanceFile.append("/");
  #endif
  chartPerformanceFile.append("chartPerformance.png");
  chartView->grab().save(chartPerformanceFile);
}

void Genet::Report::createEquityChart()
{
  QChart* chart = new QChart();

  chart->setTitle("Evolução dos indicadores de Patrimônio");
  chart->legend()->setVisible(true);

  QStringList mylist = conn.get_exercise_list(cvm);
  std::reverse(mylist.begin(), mylist.end());
  auto xaxis = new QBarCategoryAxis;
  xaxis->setCategories(mylist);
  xaxis->setLabelsAngle(80);
 
  auto barSeries = new QBarSeries();
  auto set = new QBarSet("Patrimônio Líquido");
  set->append(conn.getSeries(cvm, false, type, Dfp::DFP_INDICATOR_PL));
  barSeries->append(set);
  set = new QBarSet("Dívida Bruta");
  set->append(conn.getSeries(cvm, false ,type, Dfp::DFP_INDICATOR_LIAB));
  barSeries->append(set);
  auto lineYAxis  = new QValueAxis();
  auto barYAxis = new QValueAxis();
  chart->addSeries(barSeries);
  chart->addAxis(lineYAxis, Qt::AlignRight);
  chart->addAxis(barYAxis, Qt::AlignLeft);
  chart->addAxis(xaxis, Qt::AlignBottom);
  barSeries->attachAxis(xaxis);
  barSeries->attachAxis(barYAxis);
 
  auto lineSeries = new QSplineSeries();
  lineSeries->setName("Liquidez Corrente");
  QList<qreal> listMB(conn.getSeries(cvm,false, type, 
        Dfp::DFP_INDICATOR_LIQCORR));
  for (int i = 0; i< listMB.count(); i++)
    lineSeries->append(i,listMB.at(i));
  chart->addSeries(lineSeries);
  lineSeries->attachAxis(xaxis);
  lineSeries->attachAxis(lineYAxis);

  lineSeries = new QSplineSeries();
  lineSeries->setName("Preço / Patrimônio");
  QList<qreal> listEBIT(conn.getSeries(cvm,false, type, 
        Dfp::DFP_INDICATOR_PVP));
  for (int i = 0; i< listEBIT.count(); i++)
    lineSeries->append(i,listEBIT.at(i));
  auto lineYAxis2 = new QValueAxis();
  chart->addAxis(lineYAxis2, Qt::AlignRight);
  lineYAxis2->setVisible(false);
  chart->addSeries(lineSeries);
  lineSeries->attachAxis(xaxis);
  lineSeries->attachAxis(lineYAxis2);
  qreal min = qMin(lineYAxis->min(), lineYAxis2->min());
  qreal max = qMax(lineYAxis->max(), lineYAxis2->max());

  lineYAxis->setRange(min,max);
  lineYAxis2->setRange(min,max);


  auto chartView = new QChartView(chart);
  chartView->resize(1000,600);
  if (!dir.isValid()) throw std::runtime_error ("Can't create dir\n");
  chartEquityFile = dir.path();
  #ifdef _WIN32  
  chartEquityFile.append("\\");
  #elif defined (__linux)
  chartEquityFile.append("/");
  #endif
  chartEquityFile.append("chartEquity.png");
  chartView->grab().save(chartEquityFile);
}

QString Genet::Report::getHtml() const
{
  return html;
}

