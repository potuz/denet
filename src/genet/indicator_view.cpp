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

void IndicatorDelegate::paint (QPainter *painter, 
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  if ( index.column()== 1 ) 
  {
    QString label = index.data().toString();

    QApplication::style()->drawItemText( painter, option.rect, 
        Qt::AlignLeft, option.palette, true, label );
  }
  else if ( index.column() == 2 ) 
  {
    QStyleOptionFrame lineEditOption;
    lineEditOption.rect = option.rect;
    lineEditOption.lineWidth = 2;
    lineEditOption.midLineWidth = 1;
    lineEditOption.state = QStyle::State_Sunken;
    
    QApplication::style()->drawPrimitive(QStyle::PE_FrameLineEdit, 
        &lineEditOption, painter);

    QString value = index.data().toString();
    QApplication::style()->drawItemText( painter, option.rect, 
        Qt::AlignRight, option.palette, true, value );
  }
  else 
    QStyledItemDelegate::paint(painter, option, index);
}
  
IndicatorView::IndicatorView(int cvm_, bool anual_, 
    Dfp::FinancialInfoType type_,  const GenetDatabase &conn_, 
    QWidget *parent) :
  cvm (cvm_), anual(anual_), type(type_), conn(conn_)
{

  valueDelegate = new IndicatorDelegate(this);

  priceLabel = new QLabel(tr("Indicadores de Preço"));
  priceView = new QTableView;
  priceModel = new IndicatorPriceModel (cvm, anual, type, conn);
  priceView->setModel(priceModel);
  priceView->setItemDelegate(valueDelegate);

  performanceLabel = new QLabel(tr("Indicadores de Rendimento"));
  performanceView = new QTableView;
  performanceModel = new IndicatorPerformModel (cvm, anual, type, conn);
  performanceView->setModel(performanceModel);
  performanceView->setItemDelegate(valueDelegate);

  assetsLabel = new QLabel(tr("Indicadores de Patrimônio"));
  assetsView = new QTableView;
  assetsModel = new IndicatorAssetsModel(cvm, anual, type, conn);
  assetsView->setModel(assetsModel);
  assetsView->setItemDelegate(valueDelegate);

  miscLabel = new QLabel(tr("Indicadores Variados"));
  miscView = new QTableView;
  miscModel = new IndicatorMiscModel(cvm, anual, type, conn);
  miscView->setModel(miscModel);
  miscView->setItemDelegate(valueDelegate);

  resultLabel = new QLabel(tr("Indicadores de Resultado"));
  resultView = new QTableView;
  resultModel = new IndicatorResultModel(cvm, anual, type, conn);
  resultView->setModel(resultModel);
  resultView->setItemDelegate(valueDelegate);

  cashLabel = new QLabel(tr("Indicadores de Caixa"));
  cashView = new QTableView;
  cashModel = new IndicatorCashModel(cvm, anual, type, conn);
  cashView->setModel(cashModel);
  cashView->setItemDelegate(valueDelegate);

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(priceLabel, 0,0);
  layout->addWidget(performanceLabel, 0,1);
  layout->addWidget(priceView, 1,0);
  layout->addWidget(performanceView, 1,1);
  layout->addWidget(assetsLabel, 2,0);
  layout->addWidget(cashLabel, 2,1);
  layout->addWidget(assetsView, 3,0);
  layout->addWidget(cashView, 3,1);
  layout->addWidget(resultLabel, 4,0);
  layout->addWidget(miscLabel, 4,1);
  layout->addWidget(resultView, 5,0);
  layout->addWidget(miscView, 5,1);
  setLayout(layout);
}

void IndicatorView::changedCvm(int cvm_) {}
    
void IndicatorView::changedType(Dfp::FinancialInfoType type_) {}

void IndicatorView::changedAnual(bool anual_) {}

