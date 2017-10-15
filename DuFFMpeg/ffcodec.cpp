#include "ffcodec.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

FFCodec::FFCodec(QObject *parent)  : QObject(parent)
{
    _name = "";
    _prettyName = "";
    _abilities =  Abilities(0);
}


FFCodec::FFCodec(QString name, QString prettyName, QObject *parent)  : QObject(parent)
{
    _name = name;
    _prettyName = prettyName;
    _abilities =  Abilities(0);
}


FFCodec::FFCodec(QString name, QString prettyName, Abilities abilities, QObject *parent)  : QObject(parent)
{
    _name = name;
    _prettyName = prettyName;
    _abilities = abilities;
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
    return _abilities.testFlag(Video);
}

bool FFCodec::isAudio() const
{
    return _abilities.testFlag(Audio);
}

bool FFCodec::isEncoder() const
{
    return _abilities.testFlag(Encoder);
}

bool FFCodec::isDecoder() const
{
    return _abilities.testFlag(Decoder);
}

bool FFCodec::isLossy() const
{
    return _abilities.testFlag(Lossy);
}

bool FFCodec::isLossless() const
{
    return _abilities.testFlag(Lossless);
}

bool FFCodec::isIframe() const
{
    return _abilities.testFlag(IFrame);
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
    _abilities.setFlag(Decoder,decoder);
}

void FFCodec::setEncoder(bool encoder)
{
    _abilities.setFlag(Encoder,encoder);
}

void FFCodec::setAudio(bool audio)
{
    _abilities.setFlag(Audio,audio);
}

void FFCodec::setVideo(bool video)
{
    _abilities.setFlag(Video,video);
}

void FFCodec::setLossy(bool lossy)
{
    _abilities.setFlag(Lossy,lossy);
}

void FFCodec::setLossless(bool lossless)
{
    _abilities.setFlag(Lossless,lossless);
}

void FFCodec::setIframe(bool iframe)
{
    _abilities.setFlag(IFrame,iframe);
}

void FFCodec::setAbilities(const Abilities &abilities)
{
    _abilities = abilities;
}
