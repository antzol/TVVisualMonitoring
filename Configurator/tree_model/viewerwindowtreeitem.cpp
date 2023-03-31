
#include "viewerwindowtreeitem.h"

ViewerWindowTreeItem::ViewerWindowTreeItem(int id, const QString &name, TreeItem *parent)
    : TreeItem(name, parent)
{
    itemData.resize(Column::NumberOfColumns);
    itemData[Column::Id] = id;
    type = Type::ViewerWindow;
}

