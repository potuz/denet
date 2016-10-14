/*  This file is part of denet. 
 *  denet is a tool to study Bovespa-listed companies. 
 *
 *  Copyright (c) 2016 - Potuz potuz@potuz.net
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
#include "dfp_company.h"

int Dfp::Company::last_imported_revision ( std::tm tm ) const
{
  return conn.last_imported_revision (cvm, tm);
}

void Dfp::Company::add_revision ( int revision, std::tm tm ) const
{
  conn.add_revision (cvm, revision, tm );
}

void Dfp::Company::delete_exercise ( std::tm tm ) const
{
  conn.delete_exercise (cvm, tm);
}

std::tm Dfp::Company::last_imported_exercise () const
{ 
  return conn.last_imported_exercise ( cvm );
}

float Dfp::Company::get_indicator ( Dfp::Indicator indicator ) const
{
  return conn.get_indicator ( cvm, indicator );
}

float Dfp::Company::get_indicator ( Dfp::Indicator indicator, std::tm exercise,
          bool anual, Dfp::FinancialInfoType type ) const
{
  return conn.get_indicator ( cvm, indicator, exercise, anual, type);
}

float Dfp::Company::get_indicator ( Dfp::Indicator indicator, bool anual) const
{
  return conn.get_indicator (cvm, indicator, anual );
}

float Dfp::Company::get_indicator ( Dfp::Indicator indicator, 
          Dfp::FinancialInfoType type ) const
{
  return conn.get_indicator (cvm, indicator, type );
}

void Dfp::Company::create_table () const 
{ 
  conn.create_table(cvm);
}

