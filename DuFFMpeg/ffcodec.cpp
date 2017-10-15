#include "ffcodec.h"

FFCodec::FFCodec(QString name, QString prettyName, bool video, bool encode, bool decode, bool lossy, bool lossless, bool iframe, QObject *parent)  : QObject(parent)
{
    _name = name;
    _prettyName = prettyName;
    _encoder = encode;
    _decoder = decode;
    _video = video;
    _audio = !video;
    _lossy = lossy;
    _lossless = lossless;
    _iframe = iframe;
}

const QString FFCodec::name()
{
    return _name;
}

const QString FFCodec::prettyName()
{
    return _prettyName;
}

bool FFCodec::isVideo() const
{
    return _video;
}

bool FFCodec::isAudio() const
{
    return _audio;
}

bool FFCodec::isEncoder() const
{
    return _encoder;
}

bool FFCodec::isDecoder() const
{
    return _decoder;
}

bool FFCodec::isLossy() const
{
    return _lossy;
}

bool FFCodec::isLossless() const
{
    return _lossless;
}

bool FFCodec::isIframe() const
{
    return _iframe;
}

void FFCodec::setName(const QString &name)
{
    _name = name;
}

void FFCodec::setPrettyName(const QString &prettyName)
{
    _prettyName = prettyName;
}

void FFCodec::setDecoder(bool decoder)
{
    _decoder = decoder;
}

void FFCodec::setEncoder(bool encoder)
{
    _encoder = encoder;
}

void FFCodec::setAudio(bool audio)
{
    _audio = audio;
}

void FFCodec::setVideo(bool video)
{
    _video = video;
}

void FFCodec::setLossy(bool lossy)
{
    _lossy = lossy;
}

void FFCodec::setLossless(bool lossless)
{
    _lossless = lossless;
}

void FFCodec::setIframe(bool iframe)
{
    _iframe = iframe;
}
