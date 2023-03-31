
#include "sourcefoldertreeitem.h"

SourceFolderTreeItem::SourceFolderTreeItem(const QString &name, TreeItem *parent)
    : TreeItem(name, parent)
{
    type = SourcesFolder;
}

