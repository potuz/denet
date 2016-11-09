/*  import_view.cpp 
 * 
 *  This file is part of denet. 
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
#include "dfp/dfp_cvm_file.h"
#include "genet_database.h"
#include "import_view.h" 
#include "import_model.h"
#include "import_progress_dialog.h"
#include <QHeaderView>
#include <QTemporaryDir>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QLayout>

Genet::ImportView::ImportView(int cvm, const GenetDatabase &conn, 
    QWidget *parent) : QDialog (parent), conn(conn)
{
  this->setMinimumSize(QSize(500,500));
  Genet::ImportModel *model = new Genet::ImportModel(cvm, conn, this);
  view = new QTreeView;

  view->setModel(model);
  view->setSelectionMode(QAbstractItemView::ExtendedSelection);
  view->header()->resizeSections(QHeaderView::ResizeToContents);
  connect(view->selectionModel(), 
      SIGNAL(selectionChanged(QItemSelection, QItemSelection)), 
      this,
      SLOT(processSelection(QItemSelection, QItemSelection)));


  view->setSortingEnabled(true);
  view->sortByColumn(2, Qt::DescendingOrder);

  QLabel *label = new QLabel(tr("Selecione os arquivos para descarregar")); 
  QPushButton *okButton = new QPushButton(tr("&Baixar"));
  QPushButton *cancelButton = new QPushButton(tr("&Cancelar"));
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(label);
  layout->addWidget(view);
  QHBoxLayout *hlayout = new QHBoxLayout;
  hlayout->addWidget(cancelButton); 
  hlayout->addWidget(okButton);
  layout->addLayout(hlayout);
  setLayout(layout);
  hlayout->setSpacing(20);
  hlayout->insertStretch(0);
  okButton->setDefault(true);
  connect (cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));
  connect (okButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
}

void Genet::ImportView::processSelection(const QItemSelection& selected, 
                                      const QItemSelection& deselected) const
{
    if (selected.empty())
        return;

    QItemSelectionModel* selectionModel = view->selectionModel();
    Genet::ImportModel *model = static_cast<Genet::ImportModel*>(view->model());

    QItemSelection selection = selectionModel->selection();
    QItemSelection invalid;

    Q_FOREACH(QModelIndex index, selection.indexes())
    {
      if ( model->rowCount(index) != 0)  
        invalid.select(index, index);
    }

    selectionModel->select(invalid, QItemSelectionModel::Deselect);
}

void Genet::ImportView::import(const QModelIndex &index)
{
    emit addText(
        QString("Importando arquivo %1.").arg(index.data().toString()));
    int protocol = index.data().toInt();
    QSettings settings(QCoreApplication::organizationName(), 
        QCoreApplication::applicationName());
    QString host = settings.value("host").toString();
    QString password = settings.value("password").toString();
    std::unique_ptr<GenetDatabase> conn_;
    try {
        conn_ = std::make_unique<GenetDatabase> (host, "denet", password);
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
              return;
            }
            break;
          default: throw;
                   break;

        } 
      }

  std::string filename = Dfp::Cvm::download (protocol, tempPath);
  Dfp::CvmFile cvmFile (filename);
  cvmFile.import(*conn_);
}

void Genet::ImportView::accept() 
{
  QItemSelectionModel* selectionModel = view->selectionModel();
  QModelIndexList protocolList = selectionModel->selectedRows(2); //protocol
  Genet::ImportProgressDialog *dialog = new ImportProgressDialog (this);

  QTemporaryDir dir; 
  if (!dir.isValid()) throw std::runtime_error ("Can't create dir\n");
  tempPath = dir.path().toStdString();
  #ifdef _WIN32  
  tempPath.append("\\");
  #elif defined (__linux)
  tempPath.append("/");
  #endif
  qDebug() << "ImportView::accept(): " << dir.path();


  QFutureWatcher<void> futureWatcher;
  QObject::connect(&futureWatcher, SIGNAL(finished()), dialog, SLOT(reset()));
  QObject::connect(dialog, SIGNAL(canceled()), &futureWatcher, 
      SLOT(cancel()));
  QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), 
      dialog, SLOT(setRange(int,int)));
  QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), dialog, 
      SLOT(setValue(int)));

  futureWatcher.setFuture(QtConcurrent::map(protocolList, 
        [this] (const QModelIndex &index) { import(index);}));

  QObject::connect(this, SIGNAL(addText(const QString& )), dialog, 
      SLOT(appendText (const QString&)), Qt::QueuedConnection);

  dialog->exec();

  futureWatcher.waitForFinished();

  delete dialog;
  QDialog::accept();
}

