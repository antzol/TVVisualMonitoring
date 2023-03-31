
#include "viewerwindowfoldertreeitem.h"

ViewerWindowFolderTreeItem::ViewerWindowFolderTreeItem(const QString &name, TreeItem *parent)
    : TreeItem(name, parent)
{
    type = Type::ViewerWindowsFolder;
}

