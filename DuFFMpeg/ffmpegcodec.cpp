#include "ffmpegcodec.h"

FFmpegCodec::FFmpegCodec(QString n, QString prettyN, bool v, bool e, bool d)
{
    name = n;
    prettyName = prettyN;
    encoder = e;
    decoder = d;
    video = v;
    audio = !v;
}

QString FFmpegCodec::getName()
{
    return name;
}

QString FFmpegCodec::getPrettyName()
{
    return prettyName;
}

bool FFmpegCodec::isVideo()
{
    return video;
}

bool FFmpegCodec::isAudio()
{
    return audio;
}

bool FFmpegCodec::isEncoder()
{
    return encoder;
}

bool FFmpegCodec::isDecoder()
{
    return decoder;
}
