
#include "sourcetreeitem.h"

//---------------------------------------------------------------------------------------
SourceTreeItem::SourceTreeItem(int id, const QString &name, TreeItem *parent)
    : TreeItem(name, parent)
{
    itemData.resize(Column::NumberOfColumns);
    itemData[Column::Id] = id;
    type = Type::Source;
}

//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
