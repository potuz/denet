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
#include "database_wizard.h"
#include "config.h"
#include <QtWidgets>
#include "dfp/dfp_utils.h"
#include <string>
#include "mysql_driver.h"
#include "cppconn/connection.h"
#include "cppconn/exception.h"

namespace Genet { 
  DatabaseWizard::DatabaseWizard(QWidget *parent) : QWizard(parent)
  {
    addPage(new IntroPage);
    addPage(new RootPage);
    addPage(new UserPage);
    addPage(new ConclusionPage);

    setPixmap(QWizard::BannerPixmap, QPixmap(
          IMAGEFILE_PATH"banner.png"));
    setPixmap(QWizard::BackgroundPixmap, QPixmap(
          IMAGEFILE_PATH"background.png"));

    setWindowTitle(tr("Configuração da base de dados"));
  }

  void DatabaseWizard::accept()
  {
    QSettings settings(QCoreApplication::organizationName(), 
        QCoreApplication::applicationName());
    QString host = field("host").toString();
    QString password = field("userPassword").toString();

    try{
      Dfp::setup_dbase( field("rootUser").toString().toStdString(), 
          field("rootPassword").toString().toStdString(), host.toStdString(),
          password.toStdString() );
    } 
    catch (sql::SQLException &e ) 
    {
      switch ( e.getErrorCode() ) { 
        case 1007: 
          { 
            std::unique_ptr<sql::Connection> conn;
            try { 
              sql::Driver *driver (sql::mysql::get_driver_instance()); 
              conn.reset  ( driver->connect(host.toStdString(), "denet", 
                    password.toStdString()) );
            } catch ( sql::SQLException &e ) 
            {
              auto printable = QStringLiteral ( "<p>A base de dados "
                  "<b>denet</b> existe e não podemos nos conectar com os dados "
                  "fornecidos. O erro reportado pelo "
                  "servidor é:</p>%1").arg(e.what());
              QMessageBox::critical (this, tr("mensagem critica"), printable, 
                  QMessageBox::Ok);
            }
          }
          break;
        case 1141: 
          {
            auto printable = QStringLiteral ( "<p>A base de dados existe "
                "mas o usuario administrador não existe com esse domínio, "
                "verifique o valor do servidor. O erro reportado pelo "
                "servidor é:</p>%1").arg(e.what());
            QMessageBox::critical (this, tr("mensagem critica"), printable, 
                QMessageBox::Ok);
          }
          break;
        default:
          throw;
          break;
      }
    }
    settings.setValue("host", field("host"));
    //TODO Encrypt this password!
    settings.setValue("password", field("userPassword"));
    QDialog::accept();
  }

  IntroPage::IntroPage(QWidget *parent) : QWizardPage(parent)
  {
    setTitle(tr("Introdução"));
    setPixmap (QWizard::WatermarkPixmap, QPixmap(
          IMAGEFILE_PATH"watermark1.png"));

    label = new QLabel(tr("<p>Bem vindo a <b>genet</b>, uma plataforma de "
          "análise fundamentalista das companhias negociadas na BMF&Bovespa.</p>"
          "<p>Aparentemente não temos registradas as credenciais de acesso á "
          "base de dados. Nas próximas páginas o ajudaremos a configurar "
          "a base de dados que <b>genet</b> utilizará.</p><p> Se você já tem "
          "configurado uma base de dados para <b>genet</b> com outro aplicativo "
          "(por exemplo <b>denet</b>) não se preocupe que nós cuidaremos os "
          "dados.</p>"
          "<p>Antes de começar "
          "verifique que o servidor MySQL está funcionando e que você tem "
          "acesso como administrador.</p><p>O servidor pode estar localizado "
          "em outro computador e inclusive na internet. Tenha a mão a senha "
          "do administrador (usualmente <i>root</i> ou <i>admin</i>).</p>"));
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
  }

  RootPage::RootPage(QWidget *parent) : QWizardPage(parent)
  {
    setTitle(tr("Dados do servidor"));
    setSubTitle(tr("<p>Por favor ingresse os dados do servidor MySQL</p>"));
    setPixmap (QWizard::WatermarkPixmap, QPixmap(
          IMAGEFILE_PATH"watermark2.png")); 

    hostLabel = new QLabel(tr("&Servidor:"));
    hostLineEdit = new QLineEdit;
    hostLineEdit->setPlaceholderText("localhost");
    hostLabel->setBuddy(hostLineEdit);

    rootUserLabel = new QLabel(tr("&Administrador:"));
    rootUserLineEdit = new QLineEdit;
    rootUserLineEdit->setPlaceholderText("root");
    rootUserLabel->setBuddy(rootUserLineEdit);

    rootPasswordLabel = new QLabel(tr("S&enha:"));
    rootPasswordLineEdit = new QLineEdit;
    rootPasswordLineEdit->setEchoMode(QLineEdit::Password);
    rootPasswordLabel->setBuddy(rootPasswordLineEdit);


    registerField("host", hostLineEdit);
    registerField("rootUser", rootUserLineEdit);
    registerField("rootPassword*", rootPasswordLineEdit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(hostLabel, 0,0);
    layout->addWidget(hostLineEdit, 0,1);
    layout->addWidget(rootUserLabel, 1,0);
    layout->addWidget(rootUserLineEdit, 1,1);
    layout->addWidget(rootPasswordLabel, 2,0);
    layout->addWidget(rootPasswordLineEdit,2,1);
    setLayout(layout);
  }

  bool RootPage::validatePage()
  {
    std::string user = rootUserLineEdit->text().toStdString();
    if (user.empty()) {
      user = "root";
      rootUserLineEdit->setText("root");
    }
    std::string passwd = rootPasswordLineEdit->text().toStdString();
    std::string host = hostLineEdit->text().toStdString();
    if (host.empty()) {
      host = "localhost";
      hostLineEdit->setText("localhost");
    }
    std::unique_ptr<sql::Connection> conn;
    try { 
      sql::Driver *driver (sql::mysql::get_driver_instance()); 
      conn.reset  ( driver->connect(host, user, passwd) );
    } catch ( sql::SQLException &e ) 
    {
      switch( e.getErrorCode() ) {
        case 1045: 
          QMessageBox::critical (this, tr("messagem critica"), tr("A senha "
                " ingressada é inválida! Tente novamente."), QMessageBox::Ok);
          rootPasswordLineEdit->clear();
          break;
        case 2005: 
          QMessageBox::critical (this, tr("mensagem critica"), tr("O servidor "
                "é inválido. Tente novamente."), QMessageBox::Ok);
          hostLineEdit->clear();
          hostLineEdit->setPlaceholderText("localhost");
          break;
        default: 
          auto printable = QStringLiteral ( "<p>Sem conexão para a base"
              " de dados, "
              "verifique a sua instalação de MySQL. O erro reportado pelo "
              "servidor é:</p>%1").arg(e.what());
          QMessageBox::critical( this, tr("mensaje critico"), 
              printable, QMessageBox::Ok);
          break;
      }
      return false;
    }
    return true;
  }

  UserPage::UserPage(QWidget *parent) : QWizardPage(parent)
  {
    setTitle(tr("Configuração do usuario <b>denet</b>"));
    setSubTitle(tr("Nesta página criaremos o usuário <b>denet</b> que terá "
          "acesso á base de dados. Guarde a senha para futura utilização."));
    setPixmap (QWizard::WatermarkPixmap, QPixmap(
          IMAGEFILE_PATH"watermark2.png"));

    userPasswordLabel = new QLabel (tr("&Senha:"));
    userPasswordLabel2 = new QLabel (tr("&Reitere a Senha:"));
    userPasswordLineEdit = new QLineEdit;
    userPasswordLineEdit->setEchoMode(QLineEdit::Password);
    userPasswordLineEdit2 = new QLineEdit;
    userPasswordLineEdit2->setEchoMode(QLineEdit::Password);
    userPasswordLabel->setBuddy(userPasswordLineEdit);
    userPasswordLabel2->setBuddy(userPasswordLineEdit2);

    registerField("userPassword*", userPasswordLineEdit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(userPasswordLabel, 0,0);
    layout->addWidget(userPasswordLineEdit, 0, 1);
    layout->addWidget(userPasswordLabel2, 1,0);
    layout->addWidget(userPasswordLineEdit2, 1, 1);

    setLayout(layout);
  }

  bool UserPage::validatePage()
  {
    if ( userPasswordLineEdit2->text().compare(
          userPasswordLineEdit->text()) != 0 ){
      QMessageBox::critical ( this, tr("mensagem critica"), 
          tr("As senhas não conferem, tente novamente."), QMessageBox::Ok);
      userPasswordLineEdit->clear();
      userPasswordLineEdit2->clear();
      return false;
    }
    return true;
  }

  ConclusionPage::ConclusionPage(QWidget *parent) : QWizardPage(parent)
  {
    setTitle(tr("Conclusão"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(
          IMAGEFILE_PATH"watermark2.png"));

    label = new QLabel;
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
  }

  void ConclusionPage::initializePage()
  {
    QString finishText = wizard()->buttonText(QWizard::FinishButton);
    finishText.remove('&');
    label->setText(tr("Pressione %1 para finalizar a configuração.")
        .arg(finishText));
  }
}
