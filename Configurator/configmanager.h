#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>

#include "loggable.h"

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    static ConfigManager* getInstance();

    bool setConfigActivated(int configId, bool state);
    bool removeConfig(int configId);


signals:

private:
    static void initInstance();
    static ConfigManager *instance;
    static std::once_flag initInstanceFlag;

    explicit ConfigManager(QObject *parent = nullptr);
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager operator=(const ConfigManager&) = delete;

    void initDatabase();

    Loggable loggable;
};

#endif // CONFIGMANAGER_H
