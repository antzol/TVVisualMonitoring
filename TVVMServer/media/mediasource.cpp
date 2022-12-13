#include "mediasource.h"

#include <QRegularExpression>
#include <QTimer>

//---------------------------------------------------------------------------------------
MediaSource::MediaSource(int srcId, const QString &srcName, QObject *parent)
    : QObject{parent}
    , id(srcId)
    , name(srcName)
{
    setObjectName(QString("MediaSource %1").arg(name));

    demuxer = new Demuxer();
    demuxer->setObjectName(QString("Demuxer %1").arg(name));
    demuxer->setRwTimeout(5);
    demuxThread = new QThread();
    demuxer->moveToThread(demuxThread);
    demuxThread->start();


    connect(demuxer, &Demuxer::playbackStateChanged, this, &MediaSource::processPlaybackStateChange, Qt::QueuedConnection);
    connect(demuxer, &Demuxer::startLockRequired, this, &MediaSource::startLockRequired, Qt::QueuedConnection);
    connect(demuxer, &Demuxer::sourceOpenningFailed, this, &MediaSource::processSourceOpenningFail);
}

//---------------------------------------------------------------------------------------
int MediaSource::getId() const
{
    return id;
}

//---------------------------------------------------------------------------------------
void MediaSource::setUri(const QString &srcUri, SourceType srcType)
{
    uri = srcUri;
    type = srcType;

    if (isStreamType(type))
    {
        humanReadableUri = uri.split("?").first();
    }
    else
    {
        static const QRegularExpression re{"[\\/]"};
        humanReadableUri = uri.split(re).last();
    }
}

//---------------------------------------------------------------------------------------
void MediaSource::setAutoRestartConfig(bool enabled, int interval)
{
    autoRestartEnabled = enabled;
    autoRestartInterval = interval;
}

//---------------------------------------------------------------------------------------
void MediaSource::addDecodedService(std::shared_ptr<MediaService> service)
{
    demuxer->addDecodedService(service);
}

//---------------------------------------------------------------------------------------
void MediaSource::start()
{
    manualStopped = false;
    QMetaObject::invokeMethod(demuxer, "setSourceAndStart", Qt::QueuedConnection,
                              Q_ARG(QString, uri),
                              Q_ARG(SourceType, SourceType::UDP));
}

//---------------------------------------------------------------------------------------
void MediaSource::stop()
{
    manualStopped = true;
    QMetaObject::invokeMethod(demuxer, "stop", Qt::QueuedConnection);
}

//---------------------------------------------------------------------------------------
void MediaSource::processPlaybackStateChange(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::StoppedState && autoRestartEnabled && !manualStopped)
        addRestartTask();
    emit playbackStateChanged(state);
}

//---------------------------------------------------------------------------------------
void MediaSource::processSourceOpenningFail()
{
    if (autoRestartEnabled)
        addRestartTask();
}

//---------------------------------------------------------------------------------------
void MediaSource::addRestartTask()
{
    QString msg = QString("Source receiving will restart in %1 milliseconds").arg(autoRestartInterval);
    loggable.logMessage(objectName(), QtInfoMsg, msg);
    QTimer::singleShot(autoRestartInterval, this, &MediaSource::start);
}

//---------------------------------------------------------------------------------------
