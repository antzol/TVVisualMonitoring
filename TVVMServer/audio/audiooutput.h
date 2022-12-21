#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QObject>

#include <QAudioOutput>
#include <QAudioSink>

#include "audioframe.h"

#include "audiodecoder.h"
#include "loggable.h"

class AudioOutput : public QObject
{
    Q_OBJECT
public:
    explicit AudioOutput(QObject *parent = nullptr);
    virtual ~AudioOutput();

    void reset();

public slots:
    void init(AudioDecoder *audioDecoder);
    void writeAudioSample(const std::shared_ptr<AudioFrame> audioFrame);

private:
    QAudioSink *audioSink{nullptr};
    QIODevice *audioDevice{nullptr};
    QMetaObject::Connection connection;

    Loggable loggable;
};

#endif // AUDIOOUTPUT_H
