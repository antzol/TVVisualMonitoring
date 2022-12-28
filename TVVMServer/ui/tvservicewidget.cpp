#include "tvservicewidget.h"



//---------------------------------------------------------------------------------------
TvServiceWidget::TvServiceWidget(const QString &serviceName, QWidget *parent)
    : MediaWidget(serviceName, parent)
{
    serviceType = ServiceType::TV;
    videoWidget = new QVideoWidget();

    mediaLayout = new QHBoxLayout();
    mediaLayout->setAlignment(Qt::AlignVCenter);
    mediaLayout->setSpacing(1);
    mediaLayout->addWidget(videoWidget, 1);

    serviceNameLabel->setStyleSheet("QLabel{color: white; font: bold 14px arial,sans-serif;}");

    QVBoxLayout *wgtLayout = new QVBoxLayout();
    wgtLayout->setSpacing(1);
    wgtLayout->addLayout(mediaLayout, 1);
    wgtLayout->addWidget(serviceNameLabel, 0, Qt::AlignCenter);

    audioIndicatorsLayout = new QHBoxLayout();
    audioIndicatorsLayout->setSpacing(1);
    mediaLayout->addLayout(audioIndicatorsLayout);

    setLayout(wgtLayout);
}

//---------------------------------------------------------------------------------------
QVideoSink *TvServiceWidget::getVideoSink() const
{
    return videoWidget->videoSink();
}

//---------------------------------------------------------------------------------------
void TvServiceWidget::writeVideoFrameToSink(const std::shared_ptr<VideoFrame> videoFrame)
{
    videoWidget->videoSink()->setVideoFrame(*videoFrame->getVideoFrame());
}

//---------------------------------------------------------------------------------------
