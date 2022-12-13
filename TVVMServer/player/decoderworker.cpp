#include "decoderworker.h"

DecoderWorker::DecoderWorker(AVCodecContext *context, QObject *parent)
    : QObject{parent}
    , codecContext(context)
{

}

int DecoderWorker::decodePacket(const AVPacket *pkt)
{
    int result = 0;

    // submit the packet to the decoder
    result = avcodec_send_packet(codecContext, pkt);
    if (result < 0)
    {
        loggable.logAvError(objectName(), QtWarningMsg, "Error submitting a packet for decoding.", result);
        return result;
    }

    // get all the available frames from the decoder
    while (result >= 0)
    {
        result = avcodec_receive_frame(codecContext, frame);
        if (result < 0)
        {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (result == AVERROR_EOF || result == AVERROR(EAGAIN))
                return 0;

            loggable.logAvError(objectName(), QtWarningMsg, "Error during decoding.", result);
            return result;
        }

        // write the frame data to output
        if (codecContext->codec->type == AVMEDIA_TYPE_VIDEO
                || codecContext->codec->type == AVMEDIA_TYPE_AUDIO )
//            result = outputFrame(frame);
            emit frameReady(frame);

        av_frame_unref(frame);
        if (result < 0)
            return result;
    }

    return 0;
}
