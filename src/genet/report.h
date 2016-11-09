/*  report.h 
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
#ifndef REPORT_H
#define REPORT_H
#include "genet_database.h"
#include <QString>
#include <QObject>
#include <QTemporaryDir>

namespace Genet {
  class Report {
    public:
      Report(const GenetDatabase& conn, int cvm, 
          Dfp::FinancialInfoType type = Dfp::DFP_FINANCIAL_INFO_CONSOLIDATED);
      Report(const GenetDatabase& conn, int cvm, 
          Dfp::FinancialInfoType type, 
          const QDate& date);
      QString getHtml() const;

    private:
      void createHtml();
      void createPerformanceChart();
      void createEquityChart();
      const GenetDatabase& conn;
      int cvm;
      Dfp::FinancialInfoType type;
      const QDate& date;
      QString html;
      QString chartEquityFile;
      QString chartPerformanceFile;
      QTemporaryDir dir;
  };
}
#endif
