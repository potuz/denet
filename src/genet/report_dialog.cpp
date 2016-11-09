/*  report_dialog.cpp 
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
#include "report_dialog.h"
#include <QtWidgets>
#include <QHBoxLayout>
#include <QApplication>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#endif


Genet::ReportDialog::ReportDialog(const GenetDatabase& conn, int cvm, 
    Dfp::FinancialInfoType type) : companyReport(conn,cvm,type)
{
  editor = new QTextEdit(this);
  auto okButton = new QPushButton(tr("&Ok"));
  auto printButton = new QPushButton(tr("&Imprimir"));
  okButton->setDefault(true);
  connect (okButton, &QAbstractButton::clicked, this, &QDialog::accept);
  connect (printButton, &QAbstractButton::clicked,this,&ReportDialog::print);
  auto hlayout = new QHBoxLayout;
  auto empty = new QWidget;
  hlayout->addWidget(empty,2);
  hlayout->addWidget(printButton);
  hlayout->addWidget(okButton);
  auto layout = new QVBoxLayout(this);
  layout->addWidget (editor);
  layout->addLayout(hlayout);
  editor->setHtml(companyReport.getHtml());
  editor->setReadOnly(true);
  const QRect availableGeometry = 
          QApplication::desktop()->availableGeometry(this);
  resize(availableGeometry.width() / 2, 
      availableGeometry.height()/1.3);
  move((availableGeometry.width() - width())/2,
      availableGeometry.height()/15);
  qDebug() << "ReportDialog::ReportDialog() size: " << width() << ", " << height();
}

void Genet::ReportDialog::print()
{
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrinter printer;
    printer.setPageOrientation(QPageLayout::Landscape);

    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Imprimir Reporte"));
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    editor->print(&printer);
#endif
}

