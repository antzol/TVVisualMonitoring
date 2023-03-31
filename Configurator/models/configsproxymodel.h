
#ifndef CONFIGSPROXYMODEL_H
#define CONFIGSPROXYMODEL_H

#include <QAbstractProxyModel>

#include "configfoldertreeitem.h"

class ConfigsProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    ConfigsProxyModel(QObject *parent);

    void setSourceModel(QAbstractItemModel *source) override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

private:
    ConfigFolderTreeItem *configsFolder{nullptr};
};

#endif // CONFIGSPROXYMODEL_H
