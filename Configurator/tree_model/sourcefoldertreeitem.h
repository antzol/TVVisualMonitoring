#ifndef SOURCEFOLDERTREEITEM_H
#define SOURCEFOLDERTREEITEM_H

#include "treeitem.h"

#include <QString>

class SourceFolderTreeItem : public TreeItem
{
    Q_OBJECT
public:
    SourceFolderTreeItem(const QString &name, TreeItem *parent = nullptr);
};

#endif // SOURCEFOLDERTREEITEM_H
