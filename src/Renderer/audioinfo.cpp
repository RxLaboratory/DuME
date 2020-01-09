#include "audioinfo.h"

AudioInfo::AudioInfo(QObject *parent) : QObject(parent)
{
    reInit();
}

void AudioInfo::reInit()
{
    _id = -1;
    _samplingRate = 0;
    _channels = "";
    _bitrate = 0;
    _codec = nullptr;
    _language = "";
}

int AudioInfo::samplingRate() const
{
    return _samplingRate;
}

void AudioInfo::setSamplingRate(int samplingRate)
{
    _samplingRate = samplingRate;
}

QString AudioInfo::channels() const
{
    return _channels;
}

void AudioInfo::setChannels(const QString &channels)
{
    _channels = channels;
}

qint64 AudioInfo::bitrate() const
{
    return _bitrate;
}

void AudioInfo::setBitrate(const qint64 &bitrate)
{
    _bitrate = bitrate;
}

FFCodec *AudioInfo::codec() const
{
    return _codec;
}

void AudioInfo::setCodec(FFCodec *codec)
{
    _codec = codec;
}

QString AudioInfo::language() const
{
    return _language;
}

void AudioInfo::setLanguage(const QString &language)
{
    _language = language;
}

int AudioInfo::id() const
{
    return _id;
}

void AudioInfo::setId(int id)
{
    _id = id;
}
