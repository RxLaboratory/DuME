#include "ffcodec.h"

FFCodec::FFCodec(QString n, QString prettyN, bool v, bool e, bool d)
{
    name = n;
    prettyName = prettyN;
    encoder = e;
    decoder = d;
    video = v;
    audio = !v;
}

QString FFCodec::getName()
{
    return name;
}

QString FFCodec::getPrettyName()
{
    return prettyName;
}

bool FFCodec::isVideo()
{
    return video;
}

bool FFCodec::isAudio()
{
    return audio;
}

bool FFCodec::isEncoder()
{
    return encoder;
}

bool FFCodec::isDecoder()
{
    return decoder;
}
