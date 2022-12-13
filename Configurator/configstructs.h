#ifndef CONFIGSTRUCTS_H
#define CONFIGSTRUCTS_H

#include <QString>

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
    bool isActive{false};
};

#endif // CONFIGSTRUCTS_H
