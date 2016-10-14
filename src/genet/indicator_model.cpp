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
#include "indicator_model.h"
#include "genet_database.h"
#include <QColor>
#include <QString>
#include <QBrush>
#include "dfp/dfp_exception.h"

namespace Genet { 
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
    void IndicatorModel::setDataList( int cvm_, bool anual_, 
        Dfp::FinancialInfoType type_, const QList<QPair<QString, T>> &list )
    {
      beginResetModel();
      ind_val_pair.clear();

      try { 
        conn.last_imported_exercise(cvm_);
      } catch ( Dfp::Exception &e )
      {
        if ( e.getErrorCode() == Dfp::EXCEPTION_NO_EXERCISE ) 
          return;
        throw;
      }
      cvm = cvm_;
      anual = anual_;
      type = type_;
      for ( auto i = list.begin(); i !=list.end(); ++i)
        try {
          ind_val_pair.push_back( qMakePair (i->first, 
                conn.get_indicator(cvm, i->second, anual, type) ));
        } catch (Dfp::Exception &e ) {
          if (e.getErrorCode() == Dfp::EXCEPTION_NO_ACCT)
            ind_val_pair.push_back (qMakePair (i->first, 0));
          else throw;
        }
      endResetModel();
    }

  template<class T>
    IndicatorModel::IndicatorModel( int cvm_, bool anual_, 
        Dfp::FinancialInfoType type_, const GenetDatabase &conn_, 
        const QList<QPair<QString, T>> &entrypair, QObject *parent_ ) 
    : cvm(cvm_), anual(anual_), type(type_), conn(conn_)
    {
      setDataList(cvm, anual, type, entrypair);
    }

  IndicatorModel::IndicatorModel( int cvm_, bool anual_, 
      Dfp::FinancialInfoType type_, const GenetDatabase &conn_, 
      QObject *parent) : cvm (cvm_), anual(anual_), type(type_), conn(conn_) {}


  const QList<QPair< QString, Dfp::Indicator>> IndicatorPriceModel::entrypair = 
  { {"P/L:", Dfp::DFP_INDICATOR_PE}, 
    {"P/EBIT:", Dfp::DFP_INDICATOR_PEBIT},
    {"PSR:", Dfp::DFP_INDICATOR_PSR},
    {"P/Ativos:", Dfp::DFP_INDICATOR_PASSET},
    {"P/Cap. Giro:", Dfp::DFP_INDICATOR_PWORKCAP},
    {"P/VP:", Dfp::DFP_INDICATOR_PVP},
    {"DY:", Dfp::DFP_INDICATOR_DY} };

  IndicatorPriceModel::IndicatorPriceModel( int cvm, bool anual, 
      Dfp::FinancialInfoType type, const GenetDatabase &conn, 
      QObject *parent) : IndicatorModel ( cvm, anual, type,
        conn, entrypair ) { }

  void IndicatorPriceModel::setCvm(int cvm_)
  {
    IndicatorModel::setDataList (cvm_, anual, type, entrypair);
  }

  void IndicatorPriceModel::setAnual(bool anual_)
  {
    IndicatorModel::setDataList (cvm, anual_, type, entrypair);
  }

  void IndicatorPriceModel::setType(Dfp::FinancialInfoType type_)
  {
    IndicatorModel::setDataList (cvm, anual, type_, entrypair);
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

  void IndicatorPerformModel::setCvm(int cvm_)
  {
    IndicatorModel::setDataList (cvm_, anual, type, entrypair);
  }

  void IndicatorPerformModel::setAnual(bool anual_)
  {
    IndicatorModel::setDataList (cvm, anual_, type, entrypair);
  }

  void IndicatorPerformModel::setType(Dfp::FinancialInfoType type_)
  {
    IndicatorModel::setDataList (cvm, anual, type_, entrypair);
  }


  const QList<QPair<QString, QString>> IndicatorCashModel::entrypair = {
    {"Caixa Líq. Operacional:", "6.01" }, 
    {"Caixa Líq. Investimento:", "6.02"},
    {"Caixa Líq. Financiamento:", "6.03"},
    {"Variação Cambial:", "6.04"},
    {"Variação Caixa e Equiv.:", "6.05"},
  };

  IndicatorCashModel::IndicatorCashModel( int cvm, bool anual, 
      Dfp::FinancialInfoType type, const GenetDatabase &conn, 
      QObject *parent) : IndicatorModel ( cvm, anual, type,
        conn, entrypair ) { }

  void IndicatorCashModel::setCvm(int cvm_)
  {
    IndicatorModel::setDataList (cvm_, anual, type, entrypair);
  }

  void IndicatorCashModel::setAnual(bool anual_)
  {
    IndicatorModel::setDataList (cvm, anual_, type, entrypair);
  }

  void IndicatorCashModel::setType(Dfp::FinancialInfoType type_)
  {
    IndicatorModel::setDataList (cvm, anual, type_, entrypair);
  }


  const QList<QPair<QString, QString>> IndicatorAssetsModel::entrypair = 
  { {"Ativos:", "1"}, 
    {"Ativo Circulante:", "1.01" },
    {"Caixa Líquida:", "1.02"},
    {"Dívida CP:", "2.01.04"},
    {"Dívida LP:", "2.02.01"},
    {"Patrimônio Líq:", "2.03"},
  };

  IndicatorAssetsModel::IndicatorAssetsModel( int cvm, bool anual, 
      Dfp::FinancialInfoType type, const GenetDatabase &conn, 
      QObject *parent) : IndicatorModel ( cvm, anual, type,
        conn, entrypair ) { }

  void IndicatorAssetsModel::setCvm(int cvm_)
  {
    IndicatorModel::setDataList (cvm_, anual, type, entrypair);
  }

  void IndicatorAssetsModel::setAnual(bool anual_)
  {
    IndicatorModel::setDataList (cvm, anual_, type, entrypair);
  }

  void IndicatorAssetsModel::setType(Dfp::FinancialInfoType type_)
  {
    IndicatorModel::setDataList (cvm, anual, type_, entrypair);
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

  void IndicatorMiscModel::setCvm(int cvm_)
  {
    IndicatorModel::setDataList (cvm_, anual, type, entrypair);
  }

  void IndicatorMiscModel::setAnual(bool anual_)
  {
    IndicatorModel::setDataList (cvm, anual_, type, entrypair);
  }

  void IndicatorMiscModel::setType(Dfp::FinancialInfoType type_)
  {
    IndicatorModel::setDataList (cvm, anual, type_, entrypair);
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
    setDataList(cvm, anual, type);
  }


  void IndicatorResultModel::setDataList(int cvm, bool anual, 
      Dfp::FinancialInfoType type) 
  {
    beginResetModel();
    ind_val_pair.clear();

    try { 
      conn.last_imported_exercise(cvm);
    } catch ( Dfp::Exception &e )
    {
      if ( e.getErrorCode() == Dfp::EXCEPTION_NO_EXERCISE ) 
        return;
      throw;
    }

    int i;    
    for (i = 0; i < 3; ++i)
      try {
        ind_val_pair.push_back( qMakePair (entrypair.at(i).first, 
              conn.get_indicator(cvm, entrypair.at(i).second, false, type)));
      } catch (Dfp::Exception &e ) {
        if (e.getErrorCode() == Dfp::EXCEPTION_NO_ACCT)
          ind_val_pair.push_back (qMakePair (entrypair.at(i).first, 0));
        else throw;
      }
    for (i = 0; i < 3; ++i)
      try {
        ind_val_pair.push_back( qMakePair (entrypair.at(i+3).first, 
              conn.get_indicator(cvm, entrypair.at(i).second, true, type)));
      } catch (Dfp::Exception &e ) {
        if (e.getErrorCode() == Dfp::EXCEPTION_NO_ACCT)
          ind_val_pair.push_back (qMakePair (entrypair.at(i+3).first, 0));
        else throw;
      }
    endResetModel();
  }

  void IndicatorResultModel::setCvm (int cvm_)
  {
    cvm = cvm_;
    setDataList (cvm, anual, type);
  }

  void IndicatorResultModel::setAnual (bool anual_)
  {
    anual = anual_;
    setDataList (cvm, anual, type);
  }

  void IndicatorResultModel::setType (Dfp::FinancialInfoType type_)
  {
    type = type_;
    setDataList (cvm, anual, type);
  }
}
