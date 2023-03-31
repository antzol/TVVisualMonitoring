#include "configtreeitem.h"

#include "configmanager.h"

//---------------------------------------------------------------------------------------
ConfigTreeItem::ConfigTreeItem(int id, const QString &name, bool isActivated, TreeItem *parent)
    : TreeItem(name, parent)
{
    type = Type::Config;
    itemData.resize(Column::NumberOfColumns);
    itemData[Column::Id] = id;
    itemData[Column::IsActive] = isActivated;
}

//---------------------------------------------------------------------------------------
QVariant ConfigTreeItem::data(int column) const
{
    if (column < 0 || column >= itemData.size())
        return QVariant();

    if (column == Column::Name && itemData.at(Column::IsActive).toBool())
        return itemData.at(Column::Name).toString() + tr(" (active)");

    return itemData.at(column);
}

//---------------------------------------------------------------------------------------
//std::shared_ptr<QMenu> ConfigTreeItem::getContextMenu()
//{
//    auto menu = std::make_shared<QMenu>();

//    if (!itemData.at(Column::IsActive).toBool())
//    {
//        auto makeActiveAction = new QAction(tr("Make active"), menu.get());
//        connect(makeActiveAction, &QAction::triggered, this, &ConfigTreeItem::makeActive);
//        menu->addAction(makeActiveAction);
//    }

//    auto removeConfigAction = new QAction(tr("Remove configuration"), menu.get());
//    menu->addAction(removeConfigAction);

//    return menu;
//}

//---------------------------------------------------------------------------------------
bool ConfigTreeItem::setActivated(bool state)
{
    loggable.logMessage(QString("Config '%1'").arg(itemData.at(Column::Name).toString()),
                        QtInfoMsg,
                        QString("Change active state to %1...").arg(state));
    bool ok = ConfigManager::getInstance()->setConfigActivated(itemData.at(Column::Id).toInt(), state);
    if (ok)
    {
        itemData[Column::IsActive] = state;
    }
    else
    {
        loggable.logMessage(QString("Config '%1'").arg(itemData.at(Column::Name).toString()),
                            QtWarningMsg,
                            QString("Failed to change active state to %1.").arg(state));
    }
    return ok;
}

//---------------------------------------------------------------------------------------
bool ConfigTreeItem::remove()
{
    loggable.logMessage(QString("Config '%1'").arg(itemData.at(Column::Name).toString()),
                        QtInfoMsg,
                        "Remove configuration...");
    bool ok = ConfigManager::getInstance()->removeConfig(itemData.at(Column::Id).toInt());
    if (!ok)
    {
        loggable.logMessage(QString("Config '%1'").arg(itemData.at(Column::Name).toString()),
                            QtWarningMsg,
                            "Failed to remove configuration.");
    }
    return ok;
}

//---------------------------------------------------------------------------------------

