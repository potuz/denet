/*  chart_view.h 
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
#ifndef CHART_VIEW_H
#define CHART_VIEW_H
#include "QtWidgets"
#include "genet_database.h"

namespace Genet {
  class ChartView : public QWidget 
  {
    Q_OBJECT
    public:
      ChartView(int cvm, const GenetDatabase &conn, Dfp::FinancialInfoType 
          type = Dfp::DFP_TYPE_CONSOLIDATED, bool anual = true, 
          QWidget *parent = 0);

    public slots:
      void setCvm(int);
      void setAnual(void);
      void setType(Dfp::DfpFinancialInfoType);

    private:
      int cvm;
      bool anual;
      Dfp::FinancialInfoType type;
      const GenetDatabase &conn;
      QChart *chart;
      QGroupBox *group;
  };
};
#endif
