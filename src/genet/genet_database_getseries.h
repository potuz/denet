/*  genet_database_getseries.h 
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
#ifndef GENET_DATABASE_GETSERIES_H
#define GENET_DATABASE_GETSERIES_H

#include <QDate>
#include <QList>
#include <QStringList>
#include "dfp/dfp_exception.h"

namespace Genet {
  template<class Ind>
    QList<qreal> GenetDatabase::getSeries(int cvm, bool anual, 
        Dfp::FinancialInfoType type, const Ind &ind) const 
    {
      std::tm exer;
      exer.tm_hour=0;
      exer.tm_min=0;
      exer.tm_sec=0;

      QList<qreal> vector;
      QStringList exercises = get_exercise_list(cvm);
      foreach(const QString &str, exercises)
      {
        QDate date = QDate::fromString (str, "yyyy-MM-dd");
        exer.tm_year = date.year()-1900;
        exer.tm_mon = date.month()-1;
        exer.tm_mday = date.daysInMonth();
        
        try 
        {
          vector.push_front(get_indicator(cvm, ind, exer, anual, type));
        } catch (Dfp::Exception &e)
        {
          if (e.getErrorCode() == Dfp::EXCEPTION_NO_ACCT)
            vector.push_front(0);
          else 
            throw;
        }
      }
      return vector;
    }
}

#endif
