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
#ifndef DATABASE_WIZARD_INCLUDED
#define DATABASE_WIZARD_INCLUDED
///\file
#include <QWizard>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

namespace Genet { 
  class DatabaseWizard : public QWizard
  {
    Q_OBJECT

    public:
      DatabaseWizard(QWidget *parent=0);

      void accept() Q_DECL_OVERRIDE;
  };

  class IntroPage : public QWizardPage
  {
    Q_OBJECT
    public:
      IntroPage(QWidget *parent=0);
    private:
      QLabel *label;
  };
  class RootPage : public QWizardPage
  {
    Q_OBJECT
    public:
      RootPage(QWidget *parent=0);
    protected:
      bool validatePage() Q_DECL_OVERRIDE; 
    private:
      QLabel *hostLabel;
      QLabel *rootUserLabel;
      QLabel *rootPasswordLabel;
      QLineEdit *hostLineEdit;
      QLineEdit *rootUserLineEdit;
      QLineEdit *rootPasswordLineEdit;
  };

  class UserPage : public QWizardPage
  {
    Q_OBJECT
    public:
      UserPage(QWidget *parent=0);
    protected:
      bool validatePage() Q_DECL_OVERRIDE; 
    private:
      QLabel *userPasswordLabel;
      QLabel *userPasswordLabel2;
      QLineEdit *userPasswordLineEdit;
      QLineEdit *userPasswordLineEdit2;
  };

  class ConclusionPage : public QWizardPage
  {
    Q_OBJECT

    public:
      ConclusionPage(QWidget *parent = 0);

    protected:
      void initializePage() Q_DECL_OVERRIDE;

    private:
      QLabel *label;
  };
}
#endif
