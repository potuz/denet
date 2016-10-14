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
#include "indicator_view.h"
#include <QtWidgets>
#include "genet_database.h"
#include "dfp/dfp_exception.h"

namespace Genet { 
  void IndicatorDelegate::paint (QPainter *painter, 
      const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    QStyleOptionViewItem option2 = option;
    if (index.data(Qt::ForegroundRole).canConvert<QBrush>()) 
      option2.palette.setBrush(QPalette::WindowText, 
          qvariant_cast<QBrush>(index.data(Qt::ForegroundRole)));
    if ( index.column()== 0 ) 
    {
      QString label = index.data().toString();

      QApplication::style()->drawItemText( painter, option.rect, 
          Qt::AlignLeft, option2.palette, true, label, QPalette::WindowText );
    }
    else if ( index.column() == 1 ) 
    {
      QLocale::setDefault (QLocale (QLocale::Portuguese, QLocale::Brazil));
      QStyleOptionFrame lineEditOption;
      lineEditOption.rect = option.rect;
      lineEditOption.lineWidth = 4;
      lineEditOption.midLineWidth = 2;
      lineEditOption.state = QStyle::State_Sunken;

      QApplication::style()->drawPrimitive(QStyle::PE_FrameLineEdit, 
          &lineEditOption, painter);

      QString value;

      switch (view) {
        case Genet::PRICE_VIEW:
        case Genet::PERFORMANCE_VIEW:
          {
            double ret = index.data().toDouble();
            ret = ret * 100;
            value = QString("%L1\%").arg(ret,0, 'f', 2);
          }
          break;
        case Genet::MISC_VIEW:
          {
            double ret = index.data().toDouble();
            value = QString("%L1").arg(ret,0, 'f', 2);
          }
          break;
        default:
          {
            int ret = index.data().toInt();
            value = QString("%L1").arg(ret);
          }
          break;
      }
      QApplication::style()->drawItemText( painter, option.rect, 
          Qt::AlignRight, option2.palette, true, value, QPalette::WindowText ); 
    }
    else 
      QStyledItemDelegate::paint(painter, option, index);
  }

  IndicatorView::IndicatorView(int cvm_, bool anual_, 
      Dfp::FinancialInfoType type_,  const GenetDatabase &conn_, 
      QWidget *parent) :
    cvm (cvm_), anual(anual_), type(type_), conn(conn_)
  {
    priceDelegate = new IndicatorDelegate(Genet::PRICE_VIEW, this);
    miscDelegate = new IndicatorDelegate(Genet::MISC_VIEW, this);
    assetsDelegate = new IndicatorDelegate(Genet::ASSETS_VIEW, this);

    setStyleSheet("QTableView {background-color: transparent}");

    priceLabel = new QLabel(tr("<b>Indicadores de Preço</b>"));
    priceLabel->setAlignment(Qt::AlignCenter);
    priceView = new QTableView;
    priceModel = new IndicatorPriceModel (cvm, anual, type, conn);
    connect (this, SIGNAL(changedCvm(int)), priceModel, SLOT (setCvm(int)));
    connect (this, SIGNAL(changedAnual(bool)), priceModel, 
        SLOT (setAnual(bool)));
    connect (this, SIGNAL(changedType(Dfp::FinancialInfoType)), 
        priceModel, SLOT (setType(Dfp::FinancialInfoType)));
    priceView->setModel(priceModel);
    priceView->setItemDelegate(priceDelegate);
    priceView->setShowGrid(false);
    priceView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    priceView->horizontalHeader()->hide();
    priceView->setSelectionMode(QAbstractItemView::NoSelection);

    performanceLabel = new QLabel(tr("<b>Indicadores de Rendimento</b>"));
    performanceLabel->setAlignment(Qt::AlignCenter);
    performanceView = new QTableView;
    performanceModel = new IndicatorPerformModel (cvm, anual, type, conn);
    connect (this, SIGNAL(changedCvm(int)), performanceModel, 
        SLOT (setCvm(int)));
    connect (this, SIGNAL(changedAnual(bool)), performanceModel, 
        SLOT (setAnual(bool)));
    connect (this, SIGNAL(changedType(Dfp::FinancialInfoType)), 
        performanceModel, SLOT (setType(Dfp::FinancialInfoType)));
    performanceView->setModel(performanceModel);
    performanceView->setItemDelegate(priceDelegate);
    performanceView->setShowGrid(false);
    performanceView->horizontalHeader()->setSectionResizeMode(0,
        QHeaderView::Stretch);
    performanceView->horizontalHeader()->hide();
    performanceView->setSelectionMode(QAbstractItemView::NoSelection);

    assetsLabel = new QLabel(tr("<b>Indicadores de Patrimônio</b>"));
    assetsLabel->setAlignment(Qt::AlignCenter);
    assetsView = new QTableView;
    assetsModel = new IndicatorAssetsModel(cvm, anual, type, conn);
    connect (this, SIGNAL(changedCvm(int)), assetsModel, SLOT (setCvm(int)));
    connect (this, SIGNAL(changedAnual(bool)), assetsModel, 
        SLOT (setAnual(bool)));
    connect (this, SIGNAL(changedType(Dfp::FinancialInfoType)), 
        assetsModel, SLOT (setType(Dfp::FinancialInfoType)));
    assetsView->setModel(assetsModel);
    assetsView->setItemDelegate(assetsDelegate);
    assetsView->setShowGrid(false);
    assetsView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    assetsView->horizontalHeader()->hide();
    assetsView->setSelectionMode(QAbstractItemView::NoSelection);

    miscLabel = new QLabel(tr("<b>Indicadores Variados</b>"));
    miscLabel->setAlignment(Qt::AlignCenter);
    miscView = new QTableView;
    miscModel = new IndicatorMiscModel(cvm, anual, type, conn);
    connect (this, SIGNAL(changedCvm(int)), miscModel, SLOT (setCvm(int)));
    connect (this, SIGNAL(changedAnual(bool)), miscModel, 
        SLOT (setAnual(bool)));
    connect (this, SIGNAL(changedType(Dfp::FinancialInfoType)), 
        miscModel, SLOT (setType(Dfp::FinancialInfoType)));
    miscView->setModel(miscModel);
    miscView->setItemDelegate(miscDelegate);
    miscView->setShowGrid(false);
    miscView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    miscView->horizontalHeader()->hide();
    miscView->setSelectionMode(QAbstractItemView::NoSelection);

    resultLabel = new QLabel(tr("<b>Indicadores de Resultado</b>"));
    resultLabel->setAlignment(Qt::AlignCenter);
    resultView = new QTableView;
    resultModel = new IndicatorResultModel(cvm, anual, type, conn);
    connect (this, SIGNAL(changedCvm(int)), resultModel, SLOT (setCvm(int)));
    connect (this, SIGNAL(changedAnual(bool)), resultModel, 
        SLOT (setAnual(bool)));
    connect (this, SIGNAL(changedType(Dfp::FinancialInfoType)), 
        resultModel, SLOT (setType(Dfp::FinancialInfoType)));
    resultView->setModel(resultModel);
    resultView->setItemDelegate(assetsDelegate);
    resultView->setShowGrid(false);
    resultView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    resultView->horizontalHeader()->hide();
    resultView->setSelectionMode(QAbstractItemView::NoSelection);

    cashLabel = new QLabel(tr("<b>Indicadores de Caixa</b>"));
    cashLabel->setAlignment(Qt::AlignCenter);
    cashView = new QTableView;
    cashModel = new IndicatorCashModel(cvm, anual, type, conn);
    connect (this, SIGNAL(changedCvm(int)), cashModel, SLOT (setCvm(int)));
    connect (this, SIGNAL(changedAnual(bool)), cashModel, 
        SLOT (setAnual(bool)));
    connect (this, SIGNAL(changedType(Dfp::FinancialInfoType)), 
        cashModel, SLOT (setType(Dfp::FinancialInfoType)));
    cashView->setModel(cashModel);
    cashView->setItemDelegate(assetsDelegate);
    cashView->setShowGrid(false);
    cashView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    cashView->horizontalHeader()->hide();
    cashView->setSelectionMode(QAbstractItemView::NoSelection);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(priceLabel, 0,0);
    layout->addWidget(performanceLabel, 2,0);
    layout->addWidget(priceView, 1,0);
    layout->addWidget(performanceView, 3,0);
    layout->addWidget(assetsLabel, 0,2);
    layout->addWidget(cashLabel, 2,1);
    layout->addWidget(assetsView, 1,2);
    layout->addWidget(cashView, 3,1);
    layout->addWidget(resultLabel, 0,1);
    layout->addWidget(miscLabel, 2,2);
    layout->addWidget(resultView, 1,1);
    layout->addWidget(miscView, 3,2);
    setLayout(layout);
  }

  void IndicatorView::setCvm(int cvm_) 
  {
    cvm = cvm_;
    emit changedCvm(cvm);
  }

  void IndicatorView::setAnual(bool anual_)
  {
    anual = anual_;
    emit changedAnual(anual);
  }

  void IndicatorView::setType(Dfp::FinancialInfoType type_) 
  {
    type = type_;
    emit changedType(type);
  }
}
