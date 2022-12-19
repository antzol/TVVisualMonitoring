#include "audiooutput.h"

#include <QAudioDevice>
#include <QMediaDevices>

#include "utils.h"

//---------------------------------------------------------------------------------------
AudioOutput::AudioOutput(QObject *parent)
    : QObject{parent}
{
    setObjectName("Audio Output");
}

//---------------------------------------------------------------------------------------
AudioOutput::~AudioOutput()
{
    reset();
}

//---------------------------------------------------------------------------------------
void AudioOutput::reset()
{
    if (connection)
        QObject::disconnect(connection);

    if (audioSink)
        audioSink->deleteLater();
    audioSink = nullptr;

    if (audioDevice)
        audioDevice->deleteLater();
    audioDevice = nullptr;
}

//---------------------------------------------------------------------------------------
void AudioOutput::init(AudioDecoder *audioDecoder)
{

    QAudioFormat format = audioDecoder->audioFormat();
    QAudioDevice defaultAudioOutput(QMediaDevices::defaultAudioOutput());

    if (defaultAudioOutput.mode() == QAudioDevice::Output)
    {
        audioSink = new QAudioSink(defaultAudioOutput, format);
        QAudioFormat audioOutputFormat = audioSink->format();

        QString msg = QString("Check output audio device format (Qt):\n"
                              "- number of channels - %1\n"
                              "- sample rate - %2\n"
                              "- sample format - %3")
                .arg(audioOutputFormat.channelCount())
                .arg(audioOutputFormat.sampleRate())
                .arg(mapQSampleFormatToString(format.sampleFormat()));
        loggable.logMessage(objectName(), QtDebugMsg, msg);

        audioDevice = audioSink->start();

        connection = connect(audioDecoder, &AudioDecoder::audioSampleReady, this, &AudioOutput::writeAudioSample);
    }
}

//---------------------------------------------------------------------------------------
void AudioOutput::writeAudioSample(const std::shared_ptr<AudioFrame> audioFrame)
{
    if (audioDevice && audioSink)
        audioDevice->write(audioFrame->getData(), audioFrame->getSize());
}

//---------------------------------------------------------------------------------------
