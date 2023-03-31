#include "configfoldertreeitem.h"


//---------------------------------------------------------------------------------------
ConfigFolderTreeItem::ConfigFolderTreeItem(const QString &name, TreeItem *parent)
    : TreeItem(name, parent)
{
    type = Type::ConfigsFolder;
}

//---------------------------------------------------------------------------------------
