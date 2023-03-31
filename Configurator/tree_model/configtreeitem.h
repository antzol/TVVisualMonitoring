#ifndef CONFIGTREEITEM_H
#define CONFIGTREEITEM_H

#include <QCoreApplication>
#include <QMenu>

#include "treeitem.h"

class ConfigTreeItem : public TreeItem
{
    Q_OBJECT
public:
    enum Column {
        Name = 0,
        Id,
        IsActive,
        NumberOfColumns
    };


//    ConfigTreeItem(const QList<QVariant> &data, TreeItem *parent = nullptr);
    ConfigTreeItem(int id, const QString &name, bool isActivated, TreeItem *parent = nullptr);


    QVariant data(int column) const override;

//    std::shared_ptr<QMenu> getContextMenu() override;


public slots:
    bool setActivated(bool state);
    bool remove();

};

#endif // CONFIGTREEITEM_H
