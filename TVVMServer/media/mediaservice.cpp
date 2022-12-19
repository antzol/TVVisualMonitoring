#include "mediaservice.h"

//---------------------------------------------------------------------------------------
MediaService::MediaService(int serviceId, QObject *parent)
    : QObject{parent}
    , id(serviceId)
{

}

//---------------------------------------------------------------------------------------
void MediaService::setServiceData(int sid, ServiceType type, const QString &name)
{
    this->sid = sid;
    this->type = type;
    this->name = name;
}

//---------------------------------------------------------------------------------------
void MediaService::setWidget(MediaWidget *wgt)
{
    widget = wgt;
    if (!wgt)
        return;

    if (type == ServiceType::TV)
    {
        TvServiceWidget *tvWidget = qobject_cast<TvServiceWidget*>(widget);
        videoSink = tvWidget ? tvWidget->getVideoSink() : nullptr;
    }
}

//---------------------------------------------------------------------------------------
MediaWidget *MediaService::getMediaWidget() const
{
    return widget;
}

//---------------------------------------------------------------------------------------
QVideoSink *MediaService::getVideoSink() const
{
    return videoSink;
}

//---------------------------------------------------------------------------------------
int MediaService::getSid() const
{
    return sid;
}

//---------------------------------------------------------------------------------------
int MediaService::getId() const
{
    return id;
}

//---------------------------------------------------------------------------------------
void MediaService::setSourceId(int newSourceId)
{
    sourceId = newSourceId;
}

//---------------------------------------------------------------------------------------
int MediaService::getSourceId() const
{
    return sourceId;
}

//---------------------------------------------------------------------------------------
const QString &MediaService::getName() const
{
    return name;
}

//---------------------------------------------------------------------------------------
ServiceType MediaService::getType() const
{
    return type;
}

//---------------------------------------------------------------------------------------
void MediaService::setVideoDecodeMultithreadingConfig(int threadCount, int threadType)
{
    videoDecodeThreadCount = threadCount;
    videoDecodeThreadType = threadType;
}

//---------------------------------------------------------------------------------------
std::tuple<int, int> MediaService::getVideoDecodeMultithreadingConfig() const
{
    return std::make_tuple(videoDecodeThreadCount, videoDecodeThreadType);
}

//---------------------------------------------------------------------------------------
