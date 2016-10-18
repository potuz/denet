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
#include <QApplication>
#include <QCommandLineOption>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
#include <QMessageBox>
#include "config.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QLocale::setDefault (QLocale (QLocale::Portuguese, QLocale::Brazil));

    QCoreApplication::setOrganizationName("Death Star");
    QCoreApplication::setOrganizationDomain("potuz.net");
    QCoreApplication::setApplicationName("genet");
    QCoreApplication::setApplicationVersion(PROJECT_VERSION_COMPLETE);

    QTranslator qtTranslator;
    if (qtTranslator.load(DATAFILE_PATH"genet_pt"))
    { 
      qDebug() << "qtTranslator ok"; 
      app.installTranslator(&qtTranslator); 
    }
    
    Genet::MainWindow mainWin; mainWin.show();
    return app.exec();
}

