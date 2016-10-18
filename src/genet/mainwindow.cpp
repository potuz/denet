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
#include <QtWidgets>
#include <QRegExpValidator>
#include <QString>
#include <QSizePolicy>
#include "mainwindow.h"
#include "config.h"
#include "database_wizard.h"
#include "genet_database.h"
#include "indicator_view.h"
#include "import_view.h"
#include "balance_view.h"
#include "date_button_view.h"

namespace Genet { 
MainWindow::MainWindow() : 
  mainStackedWidget ( new QStackedWidget ),
  anual(true), 
  financial_info_type(Dfp::DFP_FINANCIAL_INFO_CONSOLIDATED)
{
  setCentralWidget (mainStackedWidget);

  readSettings();
  createActions();
  createStatusBar();
}


void MainWindow::import() 
{
  ImportView *view = new ImportView(cvm, *conn);
  view->setAttribute(Qt::WA_DeleteOnClose);
  view->exec();
  delete view;
}

void MainWindow::setFinancialInfoType (int index)
{
  int value = qobject_cast<QComboBox*>(sender())->currentIndex();
  financial_info_type = static_cast<Dfp::FinancialInfoType> (++value);

  switch (financial_info_type) {
    case  Dfp::DFP_FINANCIAL_INFO_CONSOLIDATED: 
      statusBar()->showMessage(tr("Monstrando dados consolidados."));
      break;
    case Dfp::DFP_FINANCIAL_INFO_INDIVIDUAL:
      statusBar()->showMessage(tr("Monstrando dados individuais."));
      break;
  }
  emit changedType(financial_info_type);
}

void MainWindow::setCompany()
{
  cvm = conn->get_cvm_from_ticker_str(qobject_cast<QLineEdit*>(sender())->
      text());
  QString printable = QStringLiteral(
      "Mostrando dados da companhia com cvm %1").arg(cvm);
  statusBar()->showMessage(printable);
  emit changedCvm(cvm);
}

void MainWindow::setAnual (int state)
{
  if (state == 2) {
    anual = true;
    statusBar()->showMessage(tr("Mostrando resultados anualizados"));
  }
  else { 
    statusBar()->showMessage(tr("Mostrando resultados trimestrais"));
    anual = false;
  }
  emit changedAnual(anual);
}

void MainWindow::companyTextEdited(const QString &arg1)
{
      qobject_cast<QLineEdit*>(sender())->setText(arg1.toUpper());
}

void MainWindow::wizardDB() 
{
  DatabaseWizard databaseWizard;
  databaseWizard.exec();

  QSettings settings(QCoreApplication::organizationName(), 
      QCoreApplication::applicationName());
  host = settings.value("host").toString();
  password = settings.value("password").toString();
  try {
    conn.reset( new GenetDatabase (host, "denet", password));
  } catch ( sql::SQLException &e )
  {
    switch ( e.getErrorCode() ) {
      case 1045: 
        {
          auto printable = QStringLiteral ( "<p>A senha guardada para "
              "a base de dados é incorreta. Talvez você mudou a senha "
              "utilizando outro programa?.</p>"
              "<p>Intente configurando a base de dados novamente"
              ". O erro reportado pelo "
              "servidor é:</p>%1").arg(e.what());
          QMessageBox::critical (this, tr("mensagem critica"), printable, 
              QMessageBox::Ok);
        }
        break;
      default: throw;
               break;
    } 
  }
}

void MainWindow::about()
{
  QMessageBox aboutBox; 
  aboutBox.setText(tr("<p><b>genet Copyright (C) 2016 Potuz potuz@potuz.net"
        "</b></p>"
        "<p>Uma plataforma de análise fundamentalista de empresas"
        " negociadas na BMF&Bovespa.</p>"));
  aboutBox.setInformativeText(
      tr("<p>Esse programa <b>não tem nenhuma garantia</b> e é distribuído sob"  
        " os termos da licencia \"GNU General Public License version 3\"</p>"
        "<p>Esse programa é software livre e você é "
        "permitido de redistribui-lo sob certas condições. Pressione o "
        "botão \"Licença\" para mais informações.</p>"));
  aboutBox.setStandardButtons (QMessageBox::Ok);
  aboutBox.setDefaultButton(QMessageBox::Ok);
  QFile licenseFile (DATAFILE_PATH"COPYING");
  if (!licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

  QTextStream licenseText (&licenseFile);
  QString licenseString = licenseText.readAll();
  aboutBox.setDetailedText(licenseString);
  licenseFile.close();
  aboutBox.exec();
}

void MainWindow::help()
{
}

void MainWindow::createActions()
{
  QMenu *dbMenu = menuBar()->addMenu(tr("&Database"));
  QAction *configureDatabaseAction = dbMenu->addAction(tr("&Configuração"), 
      this, &MainWindow::wizardDB); 
  configureDatabaseAction->setStatusTip(
      tr("Configurações da conexão á base de dados"));
  
  QToolBar *dbToolBar = addToolBar(tr("Database"));

  QLineEdit *companyLineEdit = new QLineEdit (dbToolBar);
  companyLineEdit->setMaxLength(6);
  companyLineEdit->setMaximumWidth(200);
  QRegExp rgx("[a-zA-Z]{4}\\d{1,2}");
  QValidator *companyValidator = new QRegExpValidator (rgx, this);
  companyLineEdit->setValidator(companyValidator);
  connect ( companyLineEdit, SIGNAL(editingFinished()), this, 
      SLOT (setCompany()));
  connect ( companyLineEdit, SIGNAL(textEdited(const QString& )), this, 
      SLOT (companyTextEdited(const QString& )));
  dbToolBar->addWidget(companyLineEdit);

  QStringList codes;
  conn->tickers(codes);
  
  QCompleter *completer = new QCompleter(codes, this);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  companyLineEdit->setCompleter(completer);

  QWidget *emptySpace = new QWidget();
  emptySpace->setMinimumWidth(20);
  dbToolBar->addWidget(emptySpace);

  QComboBox *ftypeComboBox = new QComboBox (dbToolBar);
  ftypeComboBox->addItems({"Individual","Consolidado"});
  ftypeComboBox->setCurrentIndex(1);
  dbToolBar->addWidget(ftypeComboBox);
  connect (ftypeComboBox, SIGNAL(currentIndexChanged(int)), this, 
      SLOT(setFinancialInfoType(int)));
  financial_info_type = Dfp::DFP_FINANCIAL_INFO_CONSOLIDATED;

  QWidget *emptySpace2 = new QWidget();
  emptySpace2->setMinimumWidth(20);
  dbToolBar->addWidget(emptySpace2);


  QCheckBox *anualCheckBox = new QCheckBox (tr("A&nualizado"),  dbToolBar); 
  dbToolBar->addWidget(anualCheckBox);
  connect (anualCheckBox, SIGNAL(stateChanged(int)), this, 
      SLOT(setAnual(int)));
  anualCheckBox->setChecked(true);

  QWidget *emptySpace4 = new QWidget();
  emptySpace2->setMinimumWidth(20);
  dbToolBar->addWidget(emptySpace4);
  DateButtonView *dateButton = new DateButtonView(cvm, *conn);
  dbToolBar->addWidget(dateButton);
  dateButton->setEnabled(false);

  QWidget *emptySpace3 = new QWidget();
  emptySpace3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  emptySpace3->setMinimumWidth(20);
  dbToolBar->addWidget(emptySpace3);

  const QIcon importIcon = QIcon::fromTheme("document-save", QIcon (
        IMAGEFILE_PATH"document-save.png"));
  QAction *importAction = new QAction(importIcon, tr("&Importar"), this);
  importAction->setShortcuts(QKeySequence::Save);
  importAction->setStatusTip(tr("Importa um arquivo da CVM"));
  connect(importAction, &QAction::triggered, this, &MainWindow::import);
  dbMenu->addAction(importAction);
  dbToolBar->addAction(importAction);

  const QIcon quitIcon = QIcon::fromTheme("application-exit", QIcon (
        IMAGEFILE_PATH"application-exit.png"));
  QAction *quitAction = new QAction(quitIcon, tr("&Sair"), this);
  quitAction->setStatusTip(tr("Sair do programa"));
  quitAction->setShortcuts(QKeySequence::Quit);
  connect(quitAction, &QAction::triggered, this, &QWidget::close);
  dbMenu->addAction(quitAction);
  dbToolBar->addAction(quitAction);

  QMenu *viewMenu = menuBar()->addMenu(tr("&Ver"));
//  QAction *summaryAction = viewMenu->AddAction(tr("&Resumo"));
  QAction *indicatorAction = viewMenu->addAction(tr("&Indicadores"));
  connect(indicatorAction, &QAction::triggered, [=](){
      mainStackedWidget->setCurrentIndex(INDICATOR_PAGE);});
  connect(indicatorAction, &QAction::triggered, [=](){
      dateButton->setEnabled(false);});
  QAction *assetsAction = viewMenu->addAction(tr("Balanço &Ativos"));
  connect(assetsAction, &QAction::triggered, [=](){
      mainStackedWidget->setCurrentIndex(BALANCE_PAGE);});
  connect(assetsAction, &QAction::triggered, [=](){
      dateButton->setEnabled(true);});
  QAction *liabilitiesAction = viewMenu->addAction(tr("Balanço &Passivos"));
  connect(liabilitiesAction, &QAction::triggered, [=](){
      mainStackedWidget->setCurrentIndex(BALANCE_PAGE);});
  connect(liabilitiesAction, &QAction::triggered, [=](){
      dateButton->setEnabled(true);});
  QAction *resultAction = viewMenu->addAction(tr("Demonstrativo &Resultados"));
  connect(resultAction, &QAction::triggered, [=](){
      mainStackedWidget->setCurrentIndex(BALANCE_PAGE);});
  connect(resultAction, &QAction::triggered, [=](){
      dateButton->setEnabled(true);});
  QAction *draAction = viewMenu->addAction(tr("Resultados A&brangentes"));
  connect(draAction, &QAction::triggered, [=](){
      mainStackedWidget->setCurrentIndex(BALANCE_PAGE);});
  connect(draAction, &QAction::triggered, [=](){
      dateButton->setEnabled(true);});
  QAction *dfcAction = viewMenu->addAction(tr("Fluxo de &Caixa"));
  connect(dfcAction, &QAction::triggered, [=](){
      mainStackedWidget->setCurrentIndex(BALANCE_PAGE);});
  connect(dfcAction, &QAction::triggered, [=](){
      dateButton->setEnabled(true);});
  QAction *dvaAction = viewMenu->addAction(tr("D&VA"));
  connect(dvaAction, &QAction::triggered, [=](){
      mainStackedWidget->setCurrentIndex(BALANCE_PAGE);});
  connect(dvaAction, &QAction::triggered, [=](){
      dateButton->setEnabled(true);});

  QMenu *helpMenu = menuBar()->addMenu(tr("&Ajuda"));
  QAction *aboutAction = helpMenu->addAction(tr("&Sobre"), 
      this, &MainWindow::about); 
  configureDatabaseAction->setStatusTip(
      tr("Acerca de genet"));
  QAction *helpAction = helpMenu->addAction(tr("&Ajuda"), 
      this, &MainWindow::help); 
  configureDatabaseAction->setStatusTip(
      tr("Ajuda"));

  IndicatorView *indicatorView = new IndicatorView(cvm,anual,
      financial_info_type,*conn,this);
  connect ( this, SIGNAL(changedCvm(int)), indicatorView, 
      SLOT (setCvm(int)));
  connect ( this, SIGNAL(changedAnual(bool)), indicatorView, 
      SLOT (setAnual(bool)));
  connect ( this, SIGNAL(changedType(Dfp::FinancialInfoType)), 
      indicatorView, SLOT (setType(Dfp::FinancialInfoType)));

  BalanceView *balanceView = new BalanceView(cvm, *conn, Dfp::DFP_BALANCE_BPA);
  connect ( this, SIGNAL(changedCvm(int)), balanceView, 
      SIGNAL (changedCvm(int)));
  connect ( this, SIGNAL(changedAnual(bool)), balanceView, 
      SIGNAL (changedAnual(bool)));
  connect ( this, SIGNAL(changedType(Dfp::FinancialInfoType)), 
      balanceView, SIGNAL (changedType(Dfp::FinancialInfoType)));
  connect ( this, SIGNAL(changedBalanceType(Dfp::BalanceType)), 
      balanceView, SIGNAL (changedBalanceType(Dfp::BalanceType)));
  connect (dateButton, SIGNAL(changedDate(QDate)), balanceView, 
      SIGNAL(changedDate(QDate)));

  connect(assetsAction, &QAction::triggered, [=](){
      balanceView->setBalanceType(Dfp::DFP_BALANCE_BPA);});
  connect(liabilitiesAction, &QAction::triggered, [=](){
      balanceView->setBalanceType(Dfp::DFP_BALANCE_BPP);});
  connect(dfcAction, &QAction::triggered, [=](){
      balanceView->setBalanceType(Dfp::DFP_BALANCE_DFC_MI);});
  connect(draAction, &QAction::triggered, [=](){
      balanceView->setBalanceType(Dfp::DFP_BALANCE_DRA);});
  connect(resultAction, &QAction::triggered, [=](){
      balanceView->setBalanceType(Dfp::DFP_BALANCE_DRE);});
  connect(dvaAction, &QAction::triggered, [=](){
      balanceView->setBalanceType(Dfp::DFP_BALANCE_DVA);});

  connect(this, SIGNAL(changedCvm(int)), dateButton, SLOT(setCvm(int)));

  mainStackedWidget->addWidget(indicatorView);
  mainStackedWidget->addWidget(balanceView);
  mainStackedWidget->show();
}

void MainWindow::createStatusBar()
{
  statusBar()->showMessage(tr("Nenhuma companhia selecionada"));
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), 
        QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", 
        QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = 
          QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
    host = settings.value("host").toString();
    if (host.isEmpty()) wizardDB();
    while (!conn) {
      host = settings.value("host").toString();
      password = settings.value("password").toString();
      try {
        conn = std::make_unique<GenetDatabase> (host, "denet", password);
      } catch ( sql::SQLException &e )
      {
        switch ( e.getErrorCode() ) {
          case 1045: 
            {
              auto printable = QStringLiteral ( "<p>A senha guardada para "
                  "a base de dados é incorreta. Talvez você mudou a senha "
                  "utilizando outro programa?.</p>"
                  "<p>Intente configurando a base de dados novamente"
                  ". O erro reportado pelo "
                  "servidor é:</p>%1").arg(e.what());
              QMessageBox::critical (this, tr("mensagem critica"), printable, 
                  QMessageBox::Ok);
              wizardDB();
            }
            break;
          default: throw;
                   break;

        } 
      }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(QCoreApplication::organizationName(), 
        QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}
}
