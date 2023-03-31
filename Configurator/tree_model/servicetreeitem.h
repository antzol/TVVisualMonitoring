
#ifndef SERVICETREEITEM_H
#define SERVICETREEITEM_H

#include "treeitem.h"

#include <QString>

class ServiceTreeItem : public TreeItem
{
    Q_OBJECT
public:
    enum Column {
        Name = 0,
        Id,
        NumberOfColumns
    };

    ServiceTreeItem(int id, const QString &name, TreeItem *parent = nullptr);
};

#endif // SERVICETREEITEM_H
