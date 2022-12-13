#include "configmanager.h"

#include <QApplication>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "utils.h"


ConfigManager *ConfigManager::instance = nullptr;
std::once_flag ConfigManager::initInstanceFlag;

static const std::string configDbFileName = "tvvm-config.db";
static const int UdpBufferPageSize = 4096;

//---------------------------------------------------------------------------------------
ConfigManager* ConfigManager::getInstance()
{
    std::call_once(initInstanceFlag, &ConfigManager::initInstance);
    return instance;
}

//---------------------------------------------------------------------------------------
std::optional<ConfigurationData> ConfigManager::getActiveConfiguration()
{
    loggable.logMessage(objectName(), QtDebugMsg, "Load active configuration...");

    QSqlQuery query("SELECT * FROM configuration WHERE is_active = 1 ORDER BY name LIMIT 1");

    if (!query.exec())
    {
        QString msg = QString("FAILED loading active configuration: %1").arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return std::nullopt;
    }

    if (!query.first())
    {
        loggable.logMessage(objectName(), QtWarningMsg, "ERROR: active configuration not found!");
        return std::nullopt;
    }

    ConfigurationData data;
    data.id = query.value("id").toInt();
    data.name = query.value("name").toString();
    data.isActive = query.value("is_active").toBool();

    return data;
}

//---------------------------------------------------------------------------------------
std::vector<MosaicWindowData> ConfigManager::getMosaicWindows(int configId)
{
    loggable.logMessage(objectName(), QtDebugMsg, "Load mosaic viewers...");

    std::vector<MosaicWindowData> windows;
    QString msg;

    QSqlQuery query;
    bool ok = query.prepare("SELECT * FROM mosaic_viewer WHERE configuration = :configId ORDER BY name");

    if (!ok)
    {
        msg = QString("FAILED preparing the SQL query for mosaic viewers retrieving: %1")
                .arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return windows;
    }

    query.bindValue(":configId", configId);

    if (!query.exec())
    {
        msg = QString("FAILED loading mosaic viewers: %1").arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return windows;
    }

    if (query.size() > 0)
        windows.reserve(query.size());

    while (query.next())
    {
        MosaicWindowData window;

        window.id = query.value("id").toInt();
        window.name = query.value("name").toString();
        window.width = query.value("width").toInt();
        window.height = query.value("height").toInt();

        msg = QString("Mosaic viewer loaded: \"%1\", size %2x%3")
                .arg(window.name).arg(window.width).arg(window.height);
        loggable.logMessage(objectName(), QtDebugMsg, msg);

        msg = QString("Load services for mosaic window \"%1\"").arg(window.name);
        loggable.logMessage(objectName(), QtDebugMsg, msg);

        windows.push_back(window);
    }

    return windows;
}

//---------------------------------------------------------------------------------------
std::unordered_map<int, ServiceData> ConfigManager::getServices(int mosaicWindowId)
{
    std::unordered_map<int, ServiceData> services;
    QString msg;
    QSqlQuery query;
    bool ok;

    ok = query.prepare("SELECT s.id, s.name, s.sid, s.source, s.type, smv.column, smv.row "
                       "FROM service AS s "
                       "INNER JOIN service_mosaic_viewer AS smv ON s.id = smv.service "
                       "WHERE smv.viewer = :viewerId AND s.enabled = 1 "
                       "ORDER BY s.name");

    if (!ok)
    {
        msg = QString("FAILED preparing the SQL query for services retrieving: %1")
                .arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return services;
    }

    query.bindValue(":viewerId", mosaicWindowId);

    if (!query.exec())
    {
        msg = QString("FAILED loading services: %1").arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return services;
    }

    while (query.next())
    {
        ServiceData service;
        service.id = query.value("id").toInt();
        service.name = query.value("name").toString();
        service.sid = query.value("sid").toInt();
        service.type = static_cast<ServiceType>(query.value("type").toInt());
        service.sourceId = query.value("source").toInt();

        service.windowId = mosaicWindowId;
        service.column = query.value("column").toInt();
        service.row = query.value("row").toInt();

        services[service.id] = service;

        msg = QString("Service loaded: position %1x%2, \"%3 - %4\"")
                .arg(service.column).arg(service.row)
                .arg(service.sid).arg(service.name);
        loggable.logMessage(objectName(), QtDebugMsg, msg);
    }

    return services;
}

//---------------------------------------------------------------------------------------
std::unordered_map<int, SourceData> ConfigManager::getSources(const std::vector<int> &serviceIds)
{
    loggable.logMessage(objectName(), QtDebugMsg, "Load sources...");

    std::unordered_map<int, SourceData> sources;

    bool ok;
    QString msg;
    QSqlQuery query;

    QString const queryText = "SELECT src.id, src.name, src.type, src.configuration, "
                              "src.auto_restart_enabled, src.auto_restart_interval "
                              "FROM source AS src "
                              "INNER JOIN service AS serv ON src.id = serv.source "
                              "WHERE serv.id IN (%1) "
                              "GROUP BY src.id "
                              "ORDER BY src.name";
    QVector<QString> const placeholders(serviceIds.size(), "?");

    ok = query.prepare(queryText.arg(QStringList::fromVector(placeholders).join(", ")));
    if (!ok)
    {
        msg = QString("FAILED preparing SQL query for sources retrieving:\n"
                      "- query: %1\n"
                      "- error: %2")
                .arg(query.lastQuery(), query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return sources;
    }

    for (auto const & i : serviceIds)
        query.addBindValue(i);

    if (!query.exec())
    {
        msg = QString("FAILED loading sources: %1").arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return sources;
    }

    while (query.next())
    {
        SourceData source;
        source.id = query.value("id").toInt();
        source.name = query.value("name").toString();
        source.type = static_cast<SourceType>(query.value("type").toInt());
        source.configurationId = query.value("configuration").toInt();

        source.autoRestartEnabled = query.value("auto_restart_enabled").toBool();
        source.autoRestartInterval = query.value("auto_restart_interval").toInt();

        auto builder = uriBuilders.find(source.type);
        if (builder != uriBuilders.end())
            source.uri = builder->second(source.id);

        sources[source.id] = source;

        msg = QString("Source loaded:\n"
                      "- name: %1\n"
                      "- type: %2\n"
                      "- uri: %3")
                .arg(source.name, mapSourceTypeToString(source.type), source.uri);
        loggable.logMessage(objectName(), QtDebugMsg, msg);
    }
    return sources;
}

//---------------------------------------------------------------------------------------
QString ConfigManager::getUdpSourceUri(int sourceId)
{
    bool ok;
    QString msg;

    QSqlQuery query;
    ok = query.prepare("SELECT * "
                       "FROM udp_source AS udp "
                       "WHERE udp.source = :sourceId "
                       "LIMIT 1");
    if (!ok)
    {
        msg = QString("FAILED preparing SQL query for UDP source URI retrieving:\n"
                      "- query: %1\n"
                      "- error: %2")
                .arg(query.lastQuery(), query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return {};
    }

    query.bindValue(":sourceId", sourceId);

    if (!query.exec())
    {
        msg = QString("FAILED loading UDP source: %1").arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return {};
    }

    if (!query.first())
    {
        loggable.logMessage(objectName(), QtWarningMsg, "ERROR: UDP source not found!");
        return {};
    }

    QString multicastAddress = query.value("address").toString();
    int udpPort = query.value("port").toInt();
    QString localAddress = query.value("local_address").toString();
    int bufferPagesCount = query.value("buffer_size").toInt();
    int overRunNonFatal = query.value("overrun_nonfatal").toInt();

    QString uri = QString("udp://%1:%2?fifo_size=%3&overrun_nonfatal=%4")
            .arg(multicastAddress)
            .arg(udpPort)
            .arg(bufferPagesCount * UdpBufferPageSize)
            .arg(overRunNonFatal);

    if (!localAddress.isEmpty())
        uri += QString("&localaddr=%1").arg(localAddress);
    else
        uri.insert(uri.indexOf("//")+2, '@');

    return uri;
}

//---------------------------------------------------------------------------------------
std::vector<int> ConfigManager::getServiceIdsBySource(int sourceId)
{
    std::vector<int> ids;
    QString msg;
    QSqlQuery query;
    bool ok = query.prepare("SELECT id "
                            "FROM service "
                            "WHERE source = :sourceId AND enabled = 1 "
                            "ORDER BY id");
    if (!ok)
    {
        msg = QString("FAILED preparing SQL query for service SIDs retrieving:\n"
                      "- query: %1\n"
                      "- error: %2")
                .arg(query.lastQuery(), query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return {};
    }

    query.bindValue(":sourceId", sourceId);

    if (!query.exec())
    {
        msg = QString("FAILED loading SIDs for the source: %1").arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return ids;
    }

    while (query.next())
    {
        int id = query.value("id").toInt(&ok);
        if (ok)
            ids.push_back(id);
    }

    return ids;
}

//---------------------------------------------------------------------------------------
void ConfigManager::initInstance()
{
    instance = new ConfigManager();
}

//---------------------------------------------------------------------------------------
ConfigManager::ConfigManager(QObject *parent)
    : QObject{parent}
{
    setObjectName("ConfigManager");

    uriBuilders[SourceType::UDP] = [this](int sourceId) -> QString { return getUdpSourceUri(sourceId);};


    initDatabase();
}

//---------------------------------------------------------------------------------------
void ConfigManager::initDatabase()
{
    /// TODO: create new datebase if database file doesn't exist.
    QString appFolder = QApplication::applicationDirPath() + "/";
    QString appBase = appFolder + QString::fromStdString(configDbFileName);

    loggable.logMessage(objectName(), QtDebugMsg, "Connect to the config database...");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(appBase);
    if(!db.open())
    {
        loggable.logMessage(objectName(), QtCriticalMsg, "FAILED openning config database file!");
        return;
    }

    QSqlQuery query;
    if(!query.exec("PRAGMA foreign_keys = ON"))
    {
        QString msg = QString("SQL query ERROR!\n"
                              "- query: %1\n"
                              "- error: %2")
                .arg(query.lastQuery(), query.lastError().text());
        loggable.logMessage(objectName(), QtWarningMsg, msg);
    }
}

//---------------------------------------------------------------------------------------
