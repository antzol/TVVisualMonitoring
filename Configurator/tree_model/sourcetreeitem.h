
#ifndef SOURCETREEITEM_H
#define SOURCETREEITEM_H

#include "treeitem.h"

#include <QString>

class SourceTreeItem : public TreeItem
{
    Q_OBJECT
public:
    enum Column {
        Name = 0,
        Id,
        NumberOfColumns
    };

    SourceTreeItem(int id, const QString &name, TreeItem *parent = nullptr);
};

#endif // SOURCETREEITEM_H
