
#ifndef VIEWERWINDOWFOLDERTREEITEM_H
#define VIEWERWINDOWFOLDERTREEITEM_H

#include "treeitem.h"

#include <QString>

class ViewerWindowFolderTreeItem : public TreeItem
{
    Q_OBJECT
public:
    ViewerWindowFolderTreeItem(const QString &name, TreeItem *parent = nullptr);
};

#endif // VIEWERWINDOWFOLDERTREEITEM_H
