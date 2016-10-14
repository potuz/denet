/*  import_model.cpp 
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
#include "import_model.h"
#include "genet_database.h"
#include "dfp/dfp_exception.h"

Genet::ImportModel::ImportModel(int cvm, const GenetDatabase &conn,
                                                              QObject *parent) 
  : QAbstractItemModel(parent), cvm(cvm), conn(conn)
{
  QVector<QVariant> rootData;
  rootData << tr("Exercício/Data");
  rootData << tr("Version") << tr("Protocolo");

  rootItem = new ImportItem(rootData);
  rootItem->insertChild( QVector<QVariant> {tr("ITR"), QVariant()} );
  rootItem->insertChild( QVector<QVariant> {tr("DFP"), QVariant()} );
  rootItem->insertChild( QVector<QVariant> {tr("Form. de Referencia"), 
                                                                QVariant()} );

  ImportItem *balanceItem = rootItem->child(0);
  ImportItem *dfpItem = rootItem->child(1);
  ImportItem *referenceItem = rootItem->child(2);

  try { 
    std::vector<Dfp::CvmUrl> protocolList (conn.available_for_download(
          cvm, Dfp::DFP_CVM_FILE_ITR));
    for (auto const &available : protocolList)
    {
      QVector<QVariant> childData { QString(available.date_str.c_str()),
        available.revision, available.protocol };
      balanceItem->insertChild(childData);
    }
    std::vector<Dfp::CvmUrl> dfpList (conn.available_for_download(
          cvm, Dfp::DFP_CVM_FILE_DFP));
    for (auto const &available : dfpList)
    {
      QVector<QVariant> childData { QString(available.date_str.c_str()),
        available.revision, available.protocol };
      dfpItem->insertChild(childData);
    }    
    std::vector<Dfp::CvmUrl> frList (conn.available_for_download(
          cvm, Dfp::DFP_CVM_FILE_FRE));

    for (auto const &available : frList)
    {
      QVector<QVariant> childData { QString(available.date_str.c_str()),
        available.revision, available.protocol };
      referenceItem->insertChild(childData);
    }
  }  catch ( std::invalid_argument &e ) { 
    std::cerr << "ERRO: tipo de arquivo invalido.\n";
    std::exit (1);
  } catch ( Dfp::Exception &e ) {
    if ( e.getErrorCode() == Dfp::EXCEPTION_NO_INTERNET ) {
      std::cerr << "ERRO: Sem acesso a internet. "; 
      std::cerr << "Controle a sua conexão.\n";
      exit (1);
    }
    throw;
  }
}


Genet::ImportModel::~ImportModel()
{
  delete rootItem;
}

QModelIndex Genet::ImportModel::index(int row, int column,
    const QModelIndex &parent) const 
{
  if (parent.isValid() && parent.column() != 0)
        return QModelIndex();
    ImportItem *parentItem = getItem(parent);

    ImportItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

Genet::ImportItem *Genet::ImportModel::getItem(const QModelIndex &index) const
{
  if (index.isValid()) {
    ImportItem *item = static_cast<ImportItem*>(index.internalPointer());
    if (item)
      return item;
  }
  return rootItem;
}


QModelIndex Genet::ImportModel::parent(
                              const QModelIndex &index) const 
{
  if (!index.isValid())
        return QModelIndex();

    ImportItem *childItem = getItem(index);
    ImportItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

QVariant Genet::ImportModel::data(const QModelIndex &index, 
                                              int role) const 
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    ImportItem *item = getItem(index);

    return item->data(index.column());
}

QVariant Genet::ImportModel::headerData(int section, 
                  Qt::Orientation orientation, int role) const 
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return rootItem->data(section);

  return QVariant();
}

int Genet::ImportModel::columnCount(const QModelIndex &parent) const 
{
  return rootItem->columnCount();
}

int Genet::ImportModel::rowCount(const QModelIndex &parent) const 
{
  ImportItem *parentItem = getItem(parent);
  return parentItem->childCount();
}

