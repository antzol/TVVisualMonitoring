
#ifndef VIEWERWINDOWTREEITEM_H
#define VIEWERWINDOWTREEITEM_H

#include "treeitem.h"

#include <QString>

class ViewerWindowTreeItem : public TreeItem
{
    Q_OBJECT
public:
    enum Column {
        Name = 0,
        Id,
        NumberOfColumns
    };
    
    ViewerWindowTreeItem(int id, const QString &name, TreeItem *parent = nullptr);
};

#endif // VIEWERWINDOWTREEITEM_H
