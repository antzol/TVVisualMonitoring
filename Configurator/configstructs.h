#ifndef CONFIGSTRUCTS_H
#define CONFIGSTRUCTS_H

#include <QString>

enum class CellType
{
    Empty = 0,
    TvWidget = 1,
    RadioWidgets = 2
};

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
