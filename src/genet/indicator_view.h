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
#ifndef INDICATOR_VIEW_INCLUDED
#define INDICATOR_VIEW_INCLUDED
///\file
#include <QtCore>
#include <QStyledItemDelegate>
#include "indicator_model.h"

QT_BEGIN_NAMESPACE
class QTableView;
class QLabel;
QT_END_NAMESPACE

namespace Dfp {
  enum FinancialInfoType : unsigned int;
}

namespace Genet { 
  class GenetDatabase;

  namespace Genet 
  {
    enum IndicatorViews : unsigned int {
      PRICE_VIEW = 1,
      CASH_VIEW,
      RESULT_VIEW,
      MISC_VIEW,
      ASSETS_VIEW,
      PERFORMANCE_VIEW
    };
  }

  class IndicatorDelegate : public QStyledItemDelegate 
  {
    Q_OBJECT

    public:

      IndicatorDelegate(Genet::IndicatorViews view_, QWidget *parent=0) 
        : QStyledItemDelegate(parent), view(view_) {}

      void paint (QPainter *painter, const QStyleOptionViewItem &option, 
          const QModelIndex &index) const Q_DECL_OVERRIDE;

    private:
      Genet::IndicatorViews view;
  };

  class IndicatorView : public QWidget
  {
    Q_OBJECT

    public:
      IndicatorView(int cvm_, bool anual_, Dfp::FinancialInfoType type_, 
          const GenetDatabase &conn, QWidget *parent=Q_NULLPTR);

signals:
      void changedCvm(int);
      void changedType(Dfp::FinancialInfoType);
      void changedAnual(bool);

      private slots:
        void setCvm(int cvm_);
      void setAnual(bool anual_);
      void setType(Dfp::FinancialInfoType type_);

    private:

      int cvm;
      bool anual;
      Dfp::FinancialInfoType type;
      const GenetDatabase &conn;

      QTableView *priceView;
      QTableView *cashView;
      QTableView *performanceView;
      QTableView *assetsView;
      QTableView *resultView;
      QTableView *miscView;

      QLabel *priceLabel;
      QLabel *cashLabel;
      QLabel *performanceLabel;
      QLabel *resultLabel;
      QLabel *assetsLabel;
      QLabel *miscLabel;

      IndicatorDelegate *priceDelegate;
      IndicatorDelegate *assetsDelegate;
      IndicatorDelegate *miscDelegate;
      IndicatorPriceModel *priceModel;
      IndicatorCashModel *cashModel;
      IndicatorMiscModel *miscModel;
      IndicatorResultModel *resultModel;
      IndicatorAssetsModel *assetsModel;
      IndicatorPerformModel *performanceModel;
  };
}
#endif
