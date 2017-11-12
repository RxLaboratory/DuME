#include "ffmuxer.h"

FFMuxer::FFMuxer(QObject *parent) : FFObject(parent)
{
    _defaultAudioCodec = nullptr;
    _defaultVideoCodec = nullptr;
    _name = "";
    _prettyName = "";
    _type = AudioVideo;

}

FFMuxer::FFMuxer(QString name,QObject *parent) : FFObject(parent)
{
    _name = name;
    _prettyName = "";
    _defaultAudioCodec = nullptr;
    _defaultVideoCodec = nullptr;
    _type = AudioVideo;
     _sequence = false;
}

FFMuxer::FFMuxer(QString name, QString prettyName, QObject *parent) : FFObject(parent)
{
    _name = name;
    _prettyName = prettyName;
    _defaultAudioCodec = nullptr;
    _defaultVideoCodec = nullptr;
    _type = AudioVideo;
     _sequence = false;
}

FFMuxer::FFMuxer(QString name, QString prettyName, FFMuxer::Type type, QObject *parent) : FFObject(parent)
{
    _name = name;
    _prettyName = prettyName;
    _defaultAudioCodec = nullptr;
    _defaultVideoCodec = nullptr;
    _type = type;
     _sequence = false;
}

FFCodec *FFMuxer::defaultVideoCodec() const
{
    return _defaultVideoCodec;
}

void FFMuxer::setDefaultVideoCodec(FFCodec *defaultVideoCodec)
{
    _defaultVideoCodec = defaultVideoCodec;
    checkType();
}

FFCodec *FFMuxer::defaultAudioCodec() const
{
    return _defaultAudioCodec;
}

void FFMuxer::setDefaultAudioCodec(FFCodec *defaultAudioCodec)
{
    _defaultAudioCodec = defaultAudioCodec;
    checkType();
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

bool FFMuxer::isAudio()
{
    return _type == AudioOnly || _type == AudioVideo;
}

bool FFMuxer::isVideo()
{
    return _type == VideoOnly || _type == AudioVideo;
}

bool FFMuxer::isSequence()
{
    return _sequence;
}

void FFMuxer::setType(const Type &type)
{
    _type = type;
}

QStringList FFMuxer::extensions() const
{
    return _extensions;
}

void FFMuxer::setExtensions(const QStringList &extensions)
{
    _extensions = extensions;
}

void FFMuxer::setSequence(bool sequence)
{
    _sequence = sequence;
}

void FFMuxer::checkType()
{
    if (_defaultAudioCodec == nullptr && _defaultVideoCodec != nullptr) _type = VideoOnly;
    else if (_defaultAudioCodec != nullptr && _defaultVideoCodec == nullptr) _type = AudioOnly;
    else if (_defaultAudioCodec != nullptr && _defaultVideoCodec != nullptr) _type = AudioVideo;
}
