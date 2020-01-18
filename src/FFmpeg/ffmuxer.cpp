#include "ffmuxer.h"

FFMuxer::FFMuxer(QString name, QString prettyName, QObject *parent) : FFBaseObject(name, prettyName, parent)
{
    _defaultAudioCodec = FFCodec::getDefault( this );
    _defaultVideoCodec = FFCodec::getDefault( this );
    _types = Types( 0 );
}

FFMuxer::FFMuxer(QString name, QString prettyName, Types types, QObject *parent) : FFBaseObject(name, prettyName, parent)
{
    _defaultAudioCodec = FFCodec::getDefault( this );
    _defaultVideoCodec = FFCodec::getDefault( this );
    _types = types;
}

QJsonObject FFMuxer::toJson()
{
    QJsonObject obj = FFBaseObject::toJson();
    obj.insert("sequence", isSequence());
    obj.insert("video", isVideo());
    obj.insert("audio", isAudio());
    return obj;
}

FFCodec *FFMuxer::defaultVideoCodec() const
{
    return _defaultVideoCodec;
}

void FFMuxer::setDefaultVideoCodec(FFCodec *defaultVideoCodec)
{
    if (defaultVideoCodec != nullptr && defaultVideoCodec->name() != "")
    {
        _types.setFlag(Video, true);
        _defaultVideoCodec = defaultVideoCodec;
        return;
    }
    _types.setFlag(Video, false);
    _defaultVideoCodec = FFCodec::getDefault();
}

FFCodec *FFMuxer::defaultAudioCodec() const
{
    return _defaultAudioCodec;
}

void FFMuxer::setDefaultAudioCodec(FFCodec *defaultAudioCodec)
{
    if (defaultAudioCodec != nullptr && defaultAudioCodec->name() != "")
    {
        _types.setFlag(Audio, true);
        _defaultAudioCodec = defaultAudioCodec;
        return;
    }
    _types.setFlag(Audio, false);
    _defaultAudioCodec = FFCodec::getDefault();
}

bool FFMuxer::isAudio()
{
    return _types.testFlag(Audio);
}

bool FFMuxer::isVideo()
{
    return _types.testFlag(Video);
}

bool FFMuxer::isSequence()
{
    return _types.testFlag(Sequence);
}

void FFMuxer::setAudio(bool audio)
{
    _types.setFlag(Audio, audio);
}

void FFMuxer::setVideo(bool video)
{
    _types.setFlag(Video, video);
}

QStringList FFMuxer::extensions() const
{
    return _extensions;
}

void FFMuxer::setExtensions(const QStringList &extensions)
{
    _extensions = extensions;
}

FFMuxer *FFMuxer::getDefault( QObject *parent )
{
    FFMuxer *m = new FFMuxer( "", "Default", parent );
    m->setAudio();
    m->setVideo();
    return m;
}

void FFMuxer::setSequence(bool sequence)
{
    if (sequence) _types.setFlag(Video, true);
    _types.setFlag(Sequence, sequence);
}
