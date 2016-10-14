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
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QDebug>
#include <QProgressDialog>
#include <QTemporaryDir>
#include <QThread>

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

void Genet::ImportView::accept() 
{
  QItemSelectionModel* selectionModel = view->selectionModel();
  QModelIndexList selection = selectionModel->selectedRows(2); //protocol
  QProgressDialog *progress = new QProgressDialog("Importando arquivos...", 
      "Cancelar", 0, 2*selection.size()+1);
  progress->setMinimumDuration(0);
  progress->setWindowModality(Qt::WindowModal);
  progress->setValue(0);
  QTemporaryDir dir; 
  if (!dir.isValid()) throw std::runtime_error ("Can't create dir\n");
  std::string tempPath = dir.path().toStdString();
  #ifdef _WIN32  
    tempPath.append("\\");
  #elif defined (__linux)
    tempPath.append("/");
  #endif
  for (int i = 1; i < selection.size()+1; ++i) 
  {
    progress->setValue(2*i-1);
    if (progress->wasCanceled())
      break;
    progress->setLabelText(tr("Descarregando arquivo %1 de %2 da CVM.").\
        arg(i).arg(selection.size()));
    int protocol = selection.at(i-1).data().toInt();
    std::string filename = Dfp::Cvm::download (protocol, tempPath);
    progress->setValue(2*i);
    if (progress->wasCanceled())
      break;
    progress->setLabelText(tr("Importando arquivo %1 de %2 á base de dados.").\
        arg(i).arg(selection.size()));
    Dfp::CvmFile cvmFile (filename);
    cvmFile.import(conn);
  }
  progress->setValue(2*selection.size()+1);
  delete progress;
  QDialog::accept();
}
