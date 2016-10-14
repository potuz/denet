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
#ifndef INDICATOR_MODEL_INCLUDED
#define INDICATOR_MODEL_INCLUDED
///\file
#include <QAbstractTableModel>
#include <QList>
#include <QPair>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE 

namespace Dfp 
{
  enum FinancialInfoType : unsigned int;
  enum Indicator : unsigned int;
}

namespace Genet { 
  class GenetDatabase;

  class IndicatorModel : public QAbstractTableModel 
  {
    Q_OBJECT

    public:
      template<class T>
        IndicatorModel( int cvm, bool anual, Dfp::FinancialInfoType type, 
            const GenetDatabase &conn, const QList<QPair<QString, T>> &entrypair, 
            QObject *parent = 0);

      IndicatorModel( int cvm, bool anual, Dfp::FinancialInfoType type, 
          const GenetDatabase &conn, QObject *parent = 0);

      int rowCount (const QModelIndex &parent) const Q_DECL_OVERRIDE;
      int columnCount (const QModelIndex &parent ) const Q_DECL_OVERRIDE;
      QVariant data (const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
      QVariant headerData (int section, Qt::Orientation orientation, int role)
        const Q_DECL_OVERRIDE;

    protected:
      template<class T>
        void setDataList( int cvm_, bool anual_, Dfp::FinancialInfoType type_, 
            const QList<QPair<QString, T>> &list );
      int cvm;
      bool anual;
      Dfp::FinancialInfoType type;
      const GenetDatabase &conn;
      QList<QPair<QString, QVariant>> ind_val_pair;

      private slots:
        virtual void setCvm(int) = 0;
      virtual void setAnual(bool) = 0;
      virtual void setType(Dfp::FinancialInfoType) = 0;
  };

  class IndicatorPriceModel : public IndicatorModel
  {
    Q_OBJECT

    public:
      IndicatorPriceModel(int cvm, bool anual, Dfp::FinancialInfoType type, 
          const GenetDatabase &conn, QObject *parent = 0);

      void setDataList(int cvm, bool anual, Dfp::FinancialInfoType type);

    private:
      static const QList<QPair<QString, Dfp::Indicator>> entrypair;

      private slots:
        void setCvm(int);
      void setType(Dfp::FinancialInfoType);
      void setAnual(bool);
  };

  class IndicatorPerformModel : public IndicatorModel
  {
    Q_OBJECT

    public:
      IndicatorPerformModel(int cvm, bool anual, Dfp::FinancialInfoType type, 
          const GenetDatabase &conn, QObject *parent = 0);

      void setDataList(int cvm, bool anual, Dfp::FinancialInfoType type);

    private:
      static const QList<QPair<QString, Dfp::Indicator>> entrypair;

      private slots:
        void setCvm(int);
      void setType(Dfp::FinancialInfoType);
      void setAnual(bool);
  };

  class IndicatorAssetsModel : public IndicatorModel
  {
    Q_OBJECT

    public:
      IndicatorAssetsModel(int cvm, bool anual, Dfp::FinancialInfoType type, 
          const GenetDatabase &conn, QObject *parent = 0);

      void setDataList(int cvm, bool anual, Dfp::FinancialInfoType type);

    private:
      static const QList<QPair<QString, QString>> entrypair;

      private slots:
        void setCvm(int);
      void setType(Dfp::FinancialInfoType);
      void setAnual(bool);
  };

  class IndicatorMiscModel : public IndicatorModel
  {
    Q_OBJECT

    public:
      IndicatorMiscModel(int cvm, bool anual, Dfp::FinancialInfoType type, 
          const GenetDatabase &conn, QObject *parent = 0);

      void setDataList(int cvm, bool anual, Dfp::FinancialInfoType type);

    private:
      static const QList<QPair<QString, Dfp::Indicator>> entrypair;

      private slots:
        void setCvm(int);
      void setType(Dfp::FinancialInfoType);
      void setAnual(bool);
  };

  class IndicatorCashModel : public IndicatorModel
  {
    Q_OBJECT

    public:
      IndicatorCashModel(int cvm, bool anual, Dfp::FinancialInfoType type, 
          const GenetDatabase &conn, QObject *parent = 0);

      void setDataList(int cvm, bool anual, Dfp::FinancialInfoType type);

    private:
      static const QList<QPair<QString, QString>> entrypair;

      private slots:
        void setCvm(int);
      void setType(Dfp::FinancialInfoType);
      void setAnual(bool);
  };

  class IndicatorResultModel : public IndicatorModel
  {
    Q_OBJECT

    public:
      IndicatorResultModel(int cvm, bool anual, Dfp::FinancialInfoType type, 
          const GenetDatabase &conn, QObject *parent = 0);

      void setDataList(int cvm, bool anual, Dfp::FinancialInfoType type);

    private:
      static const QList<QPair<QString, QString>> entrypair;

      private slots:
        void setCvm(int);
      void setType(Dfp::FinancialInfoType);
      void setAnual(bool);
  };
}
#endif

