#include "treemodel.h"

#include <QFont>
#include <QSqlError>
#include <QSqlQuery>

#include "configfoldertreeitem.h"
#include "viewerwindowfoldertreeitem.h"
#include "sourcefoldertreeitem.h"

#include "configtreeitem.h"
#include "servicetreeitem.h"
#include "sourcetreeitem.h"
#include "viewerwindowtreeitem.h"


//---------------------------------------------------------------------------------------
TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel{parent}
{
    setObjectName("TreeModel");
    rootItem = new TreeItem("root");

    setupModelData();
}

//---------------------------------------------------------------------------------------
TreeModel::~TreeModel()
{
    delete rootItem;
}

//---------------------------------------------------------------------------------------
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = getItem(index);

    if (role == Qt::FontRole)
    {
        if (auto configItem = dynamic_cast<ConfigTreeItem*>(item); configItem != nullptr)
        {
            bool shouldBold = configItem->data(ConfigTreeItem::IsActive).toBool();
            if (shouldBold)
            {
                QFont boldFont;
                boldFont.setBold(true);
                return boldFont;
            }
        }
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    return item->data(index.column());
}

//---------------------------------------------------------------------------------------
QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);
    return QVariant();
}

//---------------------------------------------------------------------------------------
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    TreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return QModelIndex();

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

//---------------------------------------------------------------------------------------
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem ? childItem->parent() : nullptr;

    if (parentItem == rootItem || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

//---------------------------------------------------------------------------------------
int TreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() > 0)
        return 0;

    const TreeItem *parentItem = getItem(parent);
    return parentItem ? parentItem->childCount() : 0;
}

//---------------------------------------------------------------------------------------
int TreeModel::columnCount([[maybe_unused]] const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() > 0)
        return 0;

    const TreeItem *parentItem = getItem(parent);
    if (parentItem && parentItem->childCount())
        return parentItem->child(0)->columnCount();

    return parentItem ? parentItem->childCount() : 0;
}

//---------------------------------------------------------------------------------------
TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

//---------------------------------------------------------------------------------------
ConfigFolderTreeItem *TreeModel::getConfigFolderItem() const
{
    return configsItem;
}

//---------------------------------------------------------------------------------------
void TreeModel::makeConfigActive(const QModelIndex &idx)
{
    loggable.logMessage(objectName(), QtDebugMsg, "Change active configuration...");

    auto configItem = static_cast<ConfigTreeItem*>(getItem(idx));

    auto configFolderIdx = parent(idx);
    auto configFolder = configItem->parent();

    bool ok = true;
    for (int i = 0; i < configFolder->childCount(); ++i)
    {
        ConfigTreeItem *item = static_cast<ConfigTreeItem*>(configFolder->child(i));
        if (item->childNumber() != configItem->childNumber()
                && item->data(ConfigTreeItem::Column::IsActive).toBool())
        {
            ok = item->setActivated(false);
            break;
        }
    }

    if (ok)
        configItem->setActivated(true);

    auto topLeft = index(0, 0, configFolderIdx);
    auto bottomRight = index(configFolder->childCount() - 1, configItem->columnCount() - 1, configFolderIdx);

    emit dataChanged(topLeft, bottomRight);
}

//---------------------------------------------------------------------------------------
void TreeModel::removeConfig(const QModelIndex &idx)
{
    loggable.logMessage(objectName(), QtDebugMsg, "Change active configuration...");

    auto configItem = static_cast<ConfigTreeItem*>(getItem(idx));

    auto configFolderIdx = parent(idx);
    auto configFolder = configItem->parent();

    bool ok = configItem->remove();
    if (ok)
    {
        int num = configItem->childNumber();
        beginRemoveRows(configFolderIdx, num, num);
        configFolder->removeChild(num);
        endRemoveRows();
    }

    auto topLeft = index(0, 0, configFolderIdx);
    auto bottomRight = index(configFolder->childCount() - 1, configItem->columnCount() - 1, configFolderIdx);

    emit dataChanged(topLeft, bottomRight);
}

//---------------------------------------------------------------------------------------
void TreeModel::setupModelData()
{
//    TreeItem *globalItem = new TreeItem({tr("Global settings"), TreeItem::GlobalSettings, 0});
//    TreeItem *uiItem = new TreeItem({tr("UI"), TreeItem::UiSettings, 0});
//    TreeItem *serviceWidgetItem = new TreeItem({tr("Service widgets"), TreeItem::ServiceWidgetSettings, 0});
//    TreeItem *configsItem = new TreeItem({tr("Configurations"), TreeItem::ConfigsFolder, 0});

    TreeItem *globalItem = new TreeItem(tr("Global settings"));
    TreeItem *uiItem = new TreeItem(tr("UI"));
    TreeItem *serviceWidgetItem = new TreeItem(tr("Service widgets"));
    configsItem = new ConfigFolderTreeItem(tr("Configurations"));


    rootItem->appendChild(globalItem);
    rootItem->appendChild(configsItem);

    globalItem->appendChild(uiItem);
    uiItem->appendChild(serviceWidgetItem);

    loadConfigurations(configsItem);
}

//---------------------------------------------------------------------------------------
void TreeModel::loadConfigurations(TreeItem *folderItem)
{
    QSqlQuery query("SELECT id, name, is_active FROM configuration ORDER BY id");

    if (!query.exec())
    {
        QString msg = QString("FAILED loading configurations: %1").arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    while(query.next())
    {
        QString name = query.value("name").toString();
        int id = query.value("id").toInt();
        bool isActive = query.value("is_active").toBool();
//        TreeItem *configItem = new TreeItem({name, TreeItem::Config, id});
//        ConfigTreeItem *configItem = new ConfigTreeItem({name, id, isActive});
        ConfigTreeItem *configItem= new ConfigTreeItem(id, name, isActive);
        folderItem->appendChild(configItem);

//        TreeItem *sourcesFolder = new TreeItem({tr("Sources"), TreeItem::SourcesFolder, 0});
//        TreeItem *viewportsFolder = new TreeItem({tr("Viewer windows"), TreeItem::ViewerWindowsFolder, 0});

//        TreeItem *sourcesFolder = new TreeItem(tr("Sources"));
//        TreeItem *viewportsFolder = new TreeItem(tr("Viewer windows"));

        SourceFolderTreeItem *sourcesFolder = new SourceFolderTreeItem(tr("Sources"));
        ViewerWindowFolderTreeItem *viewportsFolder = new ViewerWindowFolderTreeItem(tr("Viewer windows"));

        configItem->appendChild(sourcesFolder);
        configItem->appendChild(viewportsFolder);

        loadSources(sourcesFolder, id);
        loadViewerWindows(viewportsFolder, id);

    }
}

//---------------------------------------------------------------------------------------
void TreeModel::loadSources(TreeItem *folderItem, int configId)
{
    QSqlQuery query;
    bool ok;
    QString msg;

    ok = query.prepare("SELECT id, name FROM source WHERE configuration = :configId ORDER BY id");

    if (!ok)
    {
        msg = QString("FAILED preparing SQL query for sources retrieving:\n"
                      "- query: %1\n"
                      "- error: %2")
                .arg(query.lastQuery(), query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    query.bindValue(":configId", configId);

    if (!query.exec())
    {
        msg = QString("FAILED loading sources for the configuration: %1").arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    while(query.next())
    {
        QString name = query.value("name").toString();
        int id = query.value("id").toInt();
//        TreeItem *sourceItem = new TreeItem({name, TreeItem::Source, id});
//        TreeItem *sourceItem = new TreeItem(name);
        SourceTreeItem *sourceItem = new SourceTreeItem(id, name);
        folderItem->appendChild(sourceItem);
        loadServices(sourceItem, id);
    }
}

//---------------------------------------------------------------------------------------
void TreeModel::loadServices(TreeItem *folderItem, int sourceId)
{
    QSqlQuery query;
    bool ok;
    QString msg;

    ok = query.prepare("SELECT id, name FROM service WHERE source = :sourceId ORDER BY id");

    if (!ok)
    {
        msg = QString("FAILED preparing SQL query for services retrieving:\n"
                      "- query: %1\n"
                      "- error: %2")
                .arg(query.lastQuery(), query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    query.bindValue(":sourceId", sourceId);

    if (!query.exec())
    {
        msg = QString("FAILED loading services for the source: %1").arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    while(query.next())
    {
        QString name = query.value("name").toString();
        int id = query.value("id").toInt();
        ServiceTreeItem *serviceItem = new ServiceTreeItem(id, name);
        folderItem->appendChild(serviceItem);
    }
}

//---------------------------------------------------------------------------------------
void TreeModel::loadViewerWindows(TreeItem *folderItem, int configId)
{
    QSqlQuery query;
    bool ok;
    QString msg;

    ok = query.prepare("SELECT id, name FROM mosaic_viewer WHERE configuration = :configId ORDER BY id");

    if (!ok)
    {
        msg = QString("FAILED preparing SQL query for viewer windows retrieving:\n"
                      "- query: %1\n"
                      "- error: %2")
                .arg(query.lastQuery(), query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    query.bindValue(":configId", configId);

    if (!query.exec())
    {
        msg = QString("FAILED loading viewer windows for the configuration: %1").arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    while(query.next())
    {
        QString name = query.value("name").toString();
        int id = query.value("id").toInt();
        ViewerWindowTreeItem *viewportItem = new ViewerWindowTreeItem(id, name);
        folderItem->appendChild(viewportItem);
    }
}

//---------------------------------------------------------------------------------------
