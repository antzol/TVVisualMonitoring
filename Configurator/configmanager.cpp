#include "configmanager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

static const std::string configDbFileName = "tvvm-config.db";

ConfigManager *ConfigManager::instance = nullptr;
std::once_flag ConfigManager::initInstanceFlag;

//---------------------------------------------------------------------------------------
ConfigManager* ConfigManager::getInstance()
{
    std::call_once(initInstanceFlag, &ConfigManager::initInstance);
    return instance;
}

//---------------------------------------------------------------------------------------
bool ConfigManager::setConfigActivated(int configId, bool state)
{
    QSqlQuery query;

    bool ok = query.prepare("UPDATE configuration SET is_active = :state WHERE id = :id");
    if (!ok)
    {
        QString msg = QString("Failed to prepare query for config state changing: %1")
                .arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return false;
    }

    query.bindValue(":id", configId);
    query.bindValue(":state", state);

    if (!query.exec())
    {
        QString msg = QString("FAILED to query of config state changing: %1")
                .arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------------------
bool ConfigManager::removeConfig(int configId)
{
    QSqlQuery query;

    bool ok = query.prepare("DELETE FROM configuration WHERE id = :id");
    if (!ok)
    {
        QString msg = QString("Failed to prepare query for config removing: %1")
                          .arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return false;
    }

    query.bindValue(":id", configId);

    if (!query.exec())
    {
        QString msg = QString("FAILED query of config removing: %1")
                          .arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return false;
    }
    return true;
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

}

//---------------------------------------------------------------------------------------
void ConfigManager::initDatabase()
{
    /// TODO: create new datebase if database file doesn't exist.
    QString appBase = QString::fromStdString(configDbFileName);

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
