#ifndef TVSERVICE_H
#define TVSERVICE_H

#include "mediaservice.h"

class TvService : public MediaService
{
    Q_OBJECT
public:
    explicit TvService(int serviceId, QObject *parent = nullptr);


};

#endif // TVSERVICE_H
