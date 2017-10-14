#include "ffcodec.h"

FFCodec::FFCodec(QString n, QString prettyN, bool vid, bool encode, bool decode)
{
    name = n;
    prettyName = prettyN;
    encoder = encode;
    decoder = decode;
    video = vid;
    audio = !vid;
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

bool FFCodec::operator<(FFCodec c)
{
    return this->prettyName.toLower() < c.getPrettyName().toLower();
}

bool FFCodec::operator>(FFCodec c)
{
    return this->prettyName.toLower() > c.getPrettyName().toLower();
}
