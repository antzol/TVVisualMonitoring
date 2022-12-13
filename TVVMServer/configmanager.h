#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>

#include "configstructs.h"
#include "loggable.h"


class ConfigManager : public QObject
{
    Q_OBJECT
public:
    static ConfigManager* getInstance();

    std::optional<ConfigurationData> getActiveConfiguration();
    std::vector<MosaicWindowData> getMosaicWindows(int configId);
    std::unordered_map<int, ServiceData> getServices(int mosaicWindowId);
    std::unordered_map<int, SourceData> getSources(const std::vector<int> &serviceIds);

    QString getUdpSourceUri(int sourceId);
    std::vector<int> getServiceIdsBySource(int sourceId);

private:
    static void initInstance();
    static ConfigManager *instance;
    static std::once_flag initInstanceFlag;

    explicit ConfigManager(QObject *parent = nullptr);
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager operator=(const ConfigManager&) = delete;

    void initDatabase();

    std::unordered_map<SourceType, std::function<QString(int)>> uriBuilders;
    Loggable loggable;
};

#endif // CONFIGMANAGER_H
