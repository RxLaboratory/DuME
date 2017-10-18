#include "ffmuxer.h"

FFMuxer::FFMuxer(QObject *parent) : QObject(parent)
{

}

FFMuxer::FFMuxer(QString name,QObject *parent) : QObject(parent)
{
    _name = name;
}

FFMuxer::FFMuxer(QString name, QString prettyName, QObject *parent) : QObject(parent)
{
    _name = name;
    _prettyName = prettyName;
}

QString FFMuxer::defaultVideoCodec() const
{
    return _defaultVideoCodec;
}

void FFMuxer::setDefaultVideoCodec(const QString &defaultVideoCodec)
{
    _defaultVideoCodec = defaultVideoCodec;
}

QString FFMuxer::defaultAudioCodec() const
{
    return _defaultAudioCodec;
}

void FFMuxer::setDefaultAudioCodec(const QString &defaultAudioCodec)
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
