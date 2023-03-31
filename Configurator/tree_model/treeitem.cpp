#include "treeitem.h"

//---------------------------------------------------------------------------------------
//TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
//    : QObject(parent)
//    , itemData(data)
//    , parentItem(parent)
//{
//}

//---------------------------------------------------------------------------------------
TreeItem::TreeItem(const QString &name, TreeItem *parent)
    : QObject(parent)
    , parentItem(parent)
{
    itemData.push_back(name);
}

//---------------------------------------------------------------------------------------
TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}

//---------------------------------------------------------------------------------------
void TreeItem::setParent(TreeItem *item)
{
    parentItem = item;
}

//---------------------------------------------------------------------------------------
TreeItem *TreeItem::parent()
{
    return parentItem;
}

//---------------------------------------------------------------------------------------
TreeItem *TreeItem::child(int number)
{
    if (number < 0 || number >= childItems.size())
        return nullptr;
    return childItems.at(number);
}

//---------------------------------------------------------------------------------------
const TreeItem *TreeItem::child(int number) const
{
    if (number < 0 || number >= childItems.size())
        return nullptr;
    return childItems.at(number);
}

//---------------------------------------------------------------------------------------
int TreeItem::childCount() const
{
    return childItems.size();
}

//---------------------------------------------------------------------------------------
int TreeItem::columnCount() const
{
//    return Columns::NumberOfColumns;
    return itemData.size();
}

//---------------------------------------------------------------------------------------
void TreeItem::appendChild(TreeItem *item)
{
    item->setParent(this);
    childItems.append(item);
}

//---------------------------------------------------------------------------------------
bool TreeItem::removeChild(int position)
{
    return removeChildren(position, 1);
}

//---------------------------------------------------------------------------------------
bool TreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
    {
        delete childItems.takeAt(position);
    }

    return true;
}

//---------------------------------------------------------------------------------------
int TreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));
    return 0;
}

//---------------------------------------------------------------------------------------
QVariant TreeItem::data(int column) const
{
    if (column < 0 || column >= itemData.size())
        return QVariant();
    return itemData.at(column);
}

//---------------------------------------------------------------------------------------
bool TreeItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}

//---------------------------------------------------------------------------------------
std::shared_ptr<QMenu> TreeItem::getContextMenu()
{
    return std::shared_ptr<QMenu>();
}

//---------------------------------------------------------------------------------------
int TreeItem::getType() const
{
    return type;
}

//---------------------------------------------------------------------------------------
