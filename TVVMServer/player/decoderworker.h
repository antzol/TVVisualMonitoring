#ifndef DECODERWORKER_H
#define DECODERWORKER_H

#include <QObject>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
#include <libavdevice/avdevice.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#include "loggable.h"

class DecoderWorker : public QObject
{
    Q_OBJECT
public:
    explicit DecoderWorker(AVCodecContext *context, QObject *parent = nullptr);

public slots:
    int decodePacket(const AVPacket *pkt);

signals:
    void frameReady(AVFrame *avFrame);

private:
    AVCodecContext *codecContext;
    AVFrame *frame;

    Loggable loggable;

};

#endif // DECODERWORKER_H
