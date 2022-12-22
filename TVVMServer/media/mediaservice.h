#ifndef MEDIASERVICE_H
#define MEDIASERVICE_H

#include <QObject>

#include "configstructs.h"
#include "mediawidget.h"
#include "tvservicewidget.h"
#include "mediawidget.h"
//#include "mediaviewerwindow.h"

class MediaService : public QObject
{
    Q_OBJECT
public:
    explicit MediaService(int serviceId, QObject *parent = nullptr);

    void setServiceData(int sid, ServiceType type, const QString &name);

    void setWidget(MediaWidget *wgt);

    MediaWidget* getMediaWidget() const;

    int getSid() const;

    int getId() const;
    void setSourceId(int newSourceId);
    int getSourceId() const;

    QString getName() const;

    ServiceType getType() const;

signals:

protected:
    int id{0};
    int sid{0};
    QString name;
    ServiceType type{ServiceType::TV};

    int sourceId{0};

    MediaWidget *widget{nullptr};
    QVideoSink *videoSink{nullptr};



};

#endif // MEDIASERVICE_H
