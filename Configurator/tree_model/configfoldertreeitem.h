
#ifndef CONFIGFOLDERTREEITEM_H
#define CONFIGFOLDERTREEITEM_H

#include "treeitem.h"

#include <QString>

class ConfigFolderTreeItem : public TreeItem
{
    Q_OBJECT
public:
    ConfigFolderTreeItem(const QString &name, TreeItem *parent = nullptr);
};

#endif // CONFIGFOLDERTREEITEM_H
