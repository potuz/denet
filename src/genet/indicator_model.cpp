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
#include "indicator_model.h"
#include "genet_database.h"
#include <QColor>
#include <QString>
#include <QBrush>

int IndicatorModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return ind_val_pair.size();
}

int IndicatorModel::columnCount (const QModelIndex &parent ) const 
{
  Q_UNUSED(parent);
  return 2;
}

QVariant IndicatorModel::data (const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (index.row() >= ind_val_pair.size() || index.row() < 0)
    return QVariant();

  QPair<QString, QVariant> pair = ind_val_pair.at(index.row());
  switch (role) {
    case Qt::DisplayRole: 
      if (index.column() == 0)
        return pair.first;
      else if (index.column() == 1)
        return pair.second;
      break;
    case Qt::ForegroundRole: 
      if ( pair.second.toDouble() < 0 ) return QBrush (QColor ( "red" ));
      return QVariant();
      break;
    default: 
      break;
  }
  return QVariant();
}

QVariant IndicatorModel::headerData (int section, Qt::Orientation orientation,
    int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Indicator");

            case 1:
                return tr("Value");

            default:
                return QVariant();
        }
    }
    return QVariant();
}

template<class T>
void IndicatorModel::setCvm( int cvm, bool anual, 
    Dfp::FinancialInfoType type, const QList<QPair<QString, T>> &list )
{
  ind_val_pair.clear();
  for ( auto i = list.begin(); i !=list.end(); ++i)
    ind_val_pair.push_back( qMakePair (i->first, 
          conn.get_indicator(cvm, i->second, anual, type) ));
}

template<class T>
IndicatorModel::IndicatorModel( int cvm_, bool anual_, 
    Dfp::FinancialInfoType type_, const GenetDatabase &conn_, 
    const QList<QPair<QString, T>> &entrypair, QObject *parent_ ) 
  : cvm(cvm_), anual(anual_), type(type_), conn(conn_)
{
  setCvm(cvm, anual, type, entrypair);
}

IndicatorModel::IndicatorModel( int cvm_, bool anual_, 
    Dfp::FinancialInfoType type_, const GenetDatabase &conn_, 
    QObject *parent) : cvm (cvm_), anual(anual_), type(type_), conn(conn_) {}

 
const QList<QPair< QString, Dfp::Indicator>> IndicatorPriceModel::entrypair = 
      { {"P/L:", Dfp::DFP_INDICATOR_PE}, 
      {"P/EBIT:", Dfp::DFP_INDICATOR_PEBIT},
      {"PSR:", Dfp::DFP_INDICATOR_PSR},
      {"P/Ativos:", Dfp::DFP_INDICATOR_PASSET},
      {"P/Ativ. Circ. Líq.:", Dfp::DFP_INDICATOR_PNETLIQASSET},
      {"P/Cap. Giro:", Dfp::DFP_INDICATOR_PWORKCAP},
      {"P/VP:", Dfp::DFP_INDICATOR_PVP},
      {"DY:", Dfp::DFP_INDICATOR_DY} };

IndicatorPriceModel::IndicatorPriceModel( int cvm, bool anual, 
    Dfp::FinancialInfoType type, const GenetDatabase &conn, 
    QObject *parent) : IndicatorModel ( cvm, anual, type,
      conn, entrypair ) { }

void IndicatorPriceModel::setCvm(int cvm, bool anual, 
    Dfp::FinancialInfoType type) 
{
  IndicatorModel::setCvm (cvm, anual, type, entrypair);
}

const QList<QPair<QString, Dfp::Indicator>> IndicatorPerformModel::entrypair =
    { {"ROE:", Dfp::DFP_INDICATOR_ROE}, 
      {"ROIC:", Dfp::DFP_INDICATOR_ROIC},
      {"Margem Líquida:", Dfp::DFP_INDICATOR_PSR},
      {"Margem EBIT:", Dfp::DFP_INDICATOR_PASSET},
      {"Margem Bruta:", Dfp::DFP_INDICATOR_PNETLIQASSET}
    };
   
IndicatorPerformModel::IndicatorPerformModel( int cvm, bool anual, 
    Dfp::FinancialInfoType type, const GenetDatabase &conn, 
    QObject *parent) : IndicatorModel ( cvm, anual, type,
      conn, entrypair ) { }

void IndicatorPerformModel::setCvm(int cvm, bool anual, 
    Dfp::FinancialInfoType type) 
{
  IndicatorModel::setCvm (cvm, anual, type, entrypair);
}

const QList<QPair<QString, QString>> IndicatorCashModel::entrypair = {
     {"Caixa Líq. Operacional:", "6.01" }, 
      {"Caixa Líq. Investimento:", "6.02"},
      {"Caixa Líq. Financiamento:", "6.03"},
      {"Deprec. e Amortiz.:", "6.04"}, 
      {"Variação Cambial:", "6.05"},
      {"Variação Caixa e Equiv.:", "6.06"},
    };
 
IndicatorCashModel::IndicatorCashModel( int cvm, bool anual, 
    Dfp::FinancialInfoType type, const GenetDatabase &conn, 
    QObject *parent) : IndicatorModel ( cvm, anual, type,
      conn, entrypair ) { }

void IndicatorCashModel::setCvm(int cvm, bool anual, 
    Dfp::FinancialInfoType type) 
{
  IndicatorModel::setCvm (cvm, anual, type, entrypair);
}

const QList<QPair<QString, QString>> IndicatorAssetsModel::entrypair = 
   { {"Ativos:", "1"}, 
      {"Ativo Circulante:", "1.1" },
      {"Caixa Líquida:", "1.2"},
      {"Dívida Líquida:", "2.3"},
      {"Dívida Bruta:", "2.2"},
      {"Patrimônio Líq:", "2.1"},
    };
 
IndicatorAssetsModel::IndicatorAssetsModel( int cvm, bool anual, 
    Dfp::FinancialInfoType type, const GenetDatabase &conn, 
    QObject *parent) : IndicatorModel ( cvm, anual, type,
      conn, entrypair ) { }

void IndicatorAssetsModel::setCvm(int cvm, bool anual, 
    Dfp::FinancialInfoType type) 
{
  IndicatorModel::setCvm (cvm, anual, type, entrypair);
}

const QList<QPair<QString, Dfp::Indicator>> IndicatorMiscModel::entrypair = {
     {"Valor de Mercado:", Dfp::DFP_INDICATOR_MV }, 
      {"Valor da Empresa:", Dfp::DFP_INDICATOR_EV },
      {"Dív. Bruta / PL:", Dfp::DFP_INDICATOR_DIVBRPL},
      {"VPA:", Dfp::DFP_INDICATOR_VPA}, 
      {"Liquidez Corrente:", Dfp::DFP_INDICATOR_LIQCORR},
    };
 
IndicatorMiscModel::IndicatorMiscModel( int cvm, bool anual, 
    Dfp::FinancialInfoType type, const GenetDatabase &conn, 
    QObject *parent) : IndicatorModel ( cvm, anual, type,
      conn, entrypair ) { }

void IndicatorMiscModel::setCvm(int cvm, bool anual, 
    Dfp::FinancialInfoType type) 
{
  IndicatorModel::setCvm (cvm, anual, type, entrypair);
}

const QList<QPair<QString, QString>> IndicatorResultModel::entrypair = {
    {"Receita Líquida:", "3.01"}, 
      {"EBIT:", "3.05" },
      {"Lucro Líquido:", "3.11"},
      {"Receita Líquida (12m):", "3.01"},
      {"EBIT (12m):", "3.05"},
      {"Lucro Líquido(12m):", "3.11" },
   };
 
IndicatorResultModel::IndicatorResultModel( int cvm_, bool anual_, 
    Dfp::FinancialInfoType type_, const GenetDatabase &conn_, QObject 
    *parent) : IndicatorModel (cvm_, anual_,type_, conn_)
{
  setCvm(cvm, anual, type);
}


void IndicatorResultModel::setCvm(int cvm, bool anual, 
    Dfp::FinancialInfoType type) 
{
  ind_val_pair.clear();
  int i;    
  for (i = 0; i < 3; ++i)
    ind_val_pair.push_back( qMakePair (entrypair.at(i).first, 
     conn.get_indicator(cvm, entrypair.at(i).second, false, type)));
  for (i = 0; i < 3; ++i)
    ind_val_pair.push_back( qMakePair (entrypair.at(i).first, 
          conn.get_indicator(cvm, entrypair.at(i).second, true, type)));
}


