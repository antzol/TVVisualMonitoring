#include "configsproxymodel.h"

#include "treemodel.h"

#include "configtreeitem.h"

//---------------------------------------------------------------------------------------
ConfigsProxyModel::ConfigsProxyModel(QObject *parent)
    : QAbstractProxyModel(parent)
{

}

//---------------------------------------------------------------------------------------
void ConfigsProxyModel::setSourceModel(QAbstractItemModel *source)
{
    QAbstractProxyModel::setSourceModel(source);

    TreeModel *treeModel = qobject_cast<TreeModel*>(sourceModel());
    configsFolder = treeModel ? treeModel->getConfigFolderItem() : nullptr;
}

//---------------------------------------------------------------------------------------
QModelIndex ConfigsProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!configsFolder
        || row < 0 || row >= configsFolder->childCount()
        || column < 0 || column >= configsFolder->child(0)->columnCount())
        return QModelIndex();

    TreeItem *item = configsFolder->child(row);

    return createIndex(row, column, item);
}

//---------------------------------------------------------------------------------------
QModelIndex ConfigsProxyModel::parent(const QModelIndex &index) const
{

}

//---------------------------------------------------------------------------------------
int ConfigsProxyModel::rowCount(const QModelIndex &parent) const
{

}

//---------------------------------------------------------------------------------------
int ConfigsProxyModel::columnCount(const QModelIndex &parent) const
{

}

//---------------------------------------------------------------------------------------
QModelIndex ConfigsProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    TreeModel *treeModel = qobject_cast<TreeModel*>(sourceModel());

    if (!treeModel)
        return QModelIndex();

    ConfigFolderTreeItem *configsFolder = treeModel->getConfigFolderItem();

    if (!sourceIndex.isValid()
        || sourceIndex.row() < 0
        || sourceIndex.row() >= configsFolder->childCount()
        || sourceIndex.column() < 0
        || sourceIndex.column() >= configsFolder->columnCount())
        return QModelIndex();

    return createIndex(sourceIndex.row(), sourceIndex.column());
}

//---------------------------------------------------------------------------------------
QModelIndex ConfigsProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    QModelIndex sourceIndex;


    return sourceIndex;
}

//---------------------------------------------------------------------------------------
