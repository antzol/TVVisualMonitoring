#ifndef CONFIGSTRUCTS_H
#define CONFIGSTRUCTS_H

#include <QString>
#include <QWidget>

enum ServiceType
{
    TV = 1,
    Radio = 2
};

enum SourceType
{
    UDP = 1,
    File = 2
};

struct ConfigurationData
{
    int id;
    QString name;
    bool isActive;
};

struct ServiceData
{
    int id;
    QString name;
    int sid;
    ServiceType type;

    int sourceId;

    int windowId;
    int column;
    int row;
};

struct SourceData
{
    int id;
    QString name;
    SourceType type;
    int configurationId;
    QString uri;
    bool autoRestartEnabled;
    int autoRestartInterval;
};

struct MosaicWindowData
{
    int id;
    QString name;
    int width;
    int height;
};

#endif // CONFIGSTRUCTS_H
