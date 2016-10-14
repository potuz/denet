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
#ifndef MAINWINDOW_INCLUDED
#define MAINWINDOW_INCLUDED
///\file
#include <QMainWindow>
#include <memory> //shared_ptr
#include "genet_database.h"

QT_BEGIN_NAMESPACE
class QStackedWidget;
QT_END_NAMESPACE

namespace Dfp { 
  enum FinancialInfoType : unsigned int;
}

namespace Genet { 
class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    MainWindow();

  signals:
    void changedCvm(int);
    void changedAnual(bool);
    void changedType(Dfp::FinancialInfoType);

  private slots:
    void import();
    void download();
    void setCompany();
    void companyTextEdited (const QString& );
    void setFinancialInfoType (int );
    void setAnual(int);
    void wizardDB();
    void about();
    void help();
    void showIndicators();


  protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;


  private:
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();

    QStackedWidget *mainStackedWidget;
    std::unique_ptr<GenetDatabase> conn;
    int cvm;
    bool anual;
    Dfp::FinancialInfoType financial_info_type;

    QString host;
    QString password;
};
}
#endif

