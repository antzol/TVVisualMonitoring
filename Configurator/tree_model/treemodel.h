#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>


#include "loggable.h"

#include "treeitem.h"
#include "configfoldertreeitem.h"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit TreeModel(QObject *parent = nullptr);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    TreeItem *getItem(const QModelIndex &index) const;
    ConfigFolderTreeItem *getConfigFolderItem() const;

public slots:
    void makeConfigActive(const QModelIndex &idx);
    void removeConfig(const QModelIndex &idx);

private:

    void setupModelData();
    void loadConfigurations(TreeItem *folderItem);
    void loadSources(TreeItem *folderItem, int configId);
    void loadServices(TreeItem *folderItem, int sourceId);
    void loadViewerWindows(TreeItem *folderItem, int configId);

    TreeItem *rootItem;
    ConfigFolderTreeItem *configsItem;

    Loggable loggable;


};

#endif // TREEMODEL_H
