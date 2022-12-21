#ifndef DEMUXER_H
#define DEMUXER_H

#include <QObject>

#include <condition_variable>
#include <thread>

#include <QElapsedTimer>
#include <QMutex>

#include <QAudioOutput>
#include <QAudioSink>
#include <QVideoFrame>
#include <QVideoSink>
#include <QMediaPlayer>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include "loggable.h"

#include "audiodecoder.h"
#include "audioframe.h"
#include "audiolevelmeter.h"
#include "audiooutput.h"
#include "mediaservice.h"
#include "mediawidget.h"
#include "videodecoder.h"
#include "videoframe.h"
#include "utils.h"
#include "configstructs.h"


struct StreamInfo
{
    int index;
    int id;
    AVMediaType type;
    AVStream *stream;
    std::unordered_map<std::string, std::string> properties;
};

struct ProgramInfo {
    int findFirstStreamByType(AVMediaType type);

    AVProgram *avProgram{nullptr};
    int lastPmtVersion{-1};
    std::unordered_map<int, std::weak_ptr<StreamInfo>> streams;
    std::unordered_map<std::string, std::string> properties;
};

static int interruptCallback(void *ctx);

class Demuxer : public QObject
{
    Q_OBJECT
public:
    explicit Demuxer(QObject *parent = nullptr);
    virtual ~Demuxer();
    void setRwTimeout(int seconds);

    QMediaPlayer::PlaybackState getCurrentState() const;

    std::optional<int> getFirstProgramStreamByType(int programId, AVMediaType type);
    int getFirstStreamByType(AVMediaType type);

    void addDecodedService(std::shared_ptr<MediaService> service);
    void removeDecodedService(std::shared_ptr<MediaService> service);

    friend int interruptCallback(void *ctx);

public slots:
    void setSourceAndStart(const QString &path, SourceType type);

    void play();
    void pause();
    void stop();

    void routeServiceAudio(int sid, AudioOutput *audioOutput);

signals:
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void startLockRequired(bool locked);

    void sourceOpenningFailed();

    void currentAudioChannelsCountUpdated(int numberOfChannels);

private:
    void initPlaybackThread();
    bool prepare();

    bool findStreams();
    bool findPrograms();

    void fillProgramStreamsData(std::shared_ptr<ProgramInfo> program);

    void playing();
    void waitForReachPtsTime(AVPacket *packet);

    void notifyPlaybackState();

    bool prepareDecoders();

    std::optional<VideoDecoder*> prepareVideoDecoder(int streamIndex);
    std::optional<AudioDecoder*> prepareAudioDecoder(int streamIndex);

    void resetDecoders();
    void resetDecoder(int streamIndex);

    void reset();

    std::vector<int> findServiceIdsByStream(int streamIndex);

    QString sourcePath;
    int sourceType{-1};
    int rwTimeoutInMilliseconds{0};

    // key = program id (SID, service id)
    std::map<int, std::shared_ptr<ProgramInfo>> programs;
    // index in the vector = index of the stream in the AVFormatContext
    std::vector<std::shared_ptr<StreamInfo>> streams;

    bool ready{false};
    std::atomic<QMediaPlayer::PlaybackState> desiredState{QMediaPlayer::StoppedState};
    std::atomic<QMediaPlayer::PlaybackState> currentState{QMediaPlayer::StoppedState};
    std::condition_variable currentStateChanged;
    std::condition_variable desiredStateChanged;
    QElapsedTimer timer;
    std::mutex stateMutex;
    std::thread playbackThread;

    int64_t startDTS{-1};
    int64_t startTime{-1};

    AVFormatContext *inputFormatContext{nullptr};
    AVPacket *receivedPacket{nullptr};

    // key - stream index
    std::vector<Decoder*> decoders;

    // key - sid in the stream
    std::unordered_map<int, std::shared_ptr<MediaService>> decodedServices;

    Loggable loggable;
};

#endif // DEMUXER_H
