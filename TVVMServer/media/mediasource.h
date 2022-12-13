#ifndef MEDIASOURCE_H
#define MEDIASOURCE_H

#include <QObject>
#include <QThread>

#include "demuxer.h"
#include "loggable.h"

class MediaSource : public QObject
{
    Q_OBJECT
public:
    explicit MediaSource(int srcId, const QString &srcName, QObject *parent = nullptr);
    int getId() const;

    void setUri(const QString &srcUri, SourceType srcType);
    void setAutoRestartConfig(bool enabled, int interval);

    void addDecodedService(std::shared_ptr<MediaService> service);

public slots:
    void start();
    void stop();

    void processPlaybackStateChange(QMediaPlayer::PlaybackState state);
    void processSourceOpenningFail();

signals:
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void startLockRequired(bool locked);

private:
    void addRestartTask();

    int id;
    QString name;
    SourceType type;
    QString uri;
    QString humanReadableUri;

    Demuxer *demuxer{nullptr};
    QThread *demuxThread{nullptr};

    bool autoRestartEnabled{true};
    int autoRestartInterval{5000};
    bool manualStopped{false};

    Loggable loggable;
};

#endif // MEDIASOURCE_H
