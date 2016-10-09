/*  This file is part of denet. 
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
#include "genet_database.h"
#include <QString>

GenetDatabase::GenetDatabase (QString host, QString user, QString password)
  : Dfp::Database (host.toStdString(), user.toStdString(), 
      password.toStdString()) {}

int GenetDatabase::get_indicator (int cvm, QString account_number, bool anual,
        Dfp::FinancialInfoType type) const 
{ 
  return Dfp::Database::get_indicator (cvm, account_number.toStdString(), 
      anual, type);
}

