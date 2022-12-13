#ifndef TVSERVICEWIDGET_H
#define TVSERVICEWIDGET_H

#include <QWidget>

#include <QHBoxLayout>
#include <QLabel>

#include <QVideoWidget>
#include <QVideoSink>

#include "audiolevelwidget.h"
#include "mediawidget.h"
#include "videoframe.h"

class TvServiceWidget : public MediaWidget
{
    Q_OBJECT
public:
    explicit TvServiceWidget(const QString &serviceName, QWidget *parent = nullptr);

    QVideoSink *getVideoSink() const;

public slots:
    void writeVideoFrameToSink(const std::shared_ptr<VideoFrame> videoFrame);

private:
    QVideoWidget *videoWidget;
    QHBoxLayout *mediaLayout;
};

#endif // TVSERVICEWIDGET_H
