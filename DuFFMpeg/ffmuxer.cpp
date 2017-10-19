#include "ffmuxer.h"

FFMuxer::FFMuxer(QObject *parent) : QObject(parent)
{
    _defaultAudioCodec = nullptr;
    _defaultVideoCodec = nullptr;
    _name = "";
    _prettyName = "";
}

FFMuxer::FFMuxer(QString name,QObject *parent) : QObject(parent)
{
    _name = name;
    _prettyName = "";
    _defaultAudioCodec = nullptr;
    _defaultVideoCodec = nullptr;
}

FFMuxer::FFMuxer(QString name, QString prettyName, QObject *parent) : QObject(parent)
{
    _name = name;
    _prettyName = prettyName;
    _defaultAudioCodec = nullptr;
    _defaultVideoCodec = nullptr;
}

FFCodec *FFMuxer::defaultVideoCodec() const
{
    return _defaultVideoCodec;
}

void FFMuxer::setDefaultVideoCodec(FFCodec *defaultVideoCodec)
{
    _defaultVideoCodec = defaultVideoCodec;
}

FFCodec *FFMuxer::defaultAudioCodec() const
{
    return _defaultAudioCodec;
}

void FFMuxer::setDefaultAudioCodec(FFCodec *defaultAudioCodec)
{
    _defaultAudioCodec = defaultAudioCodec;
}

QString FFMuxer::name() const
{
    return _name;
}

void FFMuxer::setName(const QString &name)
{
    _name = name;
}

QString FFMuxer::prettyName() const
{
    return _prettyName;
}

void FFMuxer::setPrettyName(const QString &prettyName)
{
    _prettyName = prettyName;
}
