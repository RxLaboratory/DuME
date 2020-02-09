#include "videoinfo.h"

VideoInfo::VideoInfo(FFmpeg *ffmpeg, QObject *parent) : QObject(parent)
{
    _ffmpeg = ffmpeg;
    _id = -1;
    _quality = -1;
    _profile = _ffmpeg->profile("");
    _level = "";
    _pixAspect = 1;
    _pixFormat = _ffmpeg->pixFormat("");
    _bitrate = 0;
    _framerate = 0.0;
    _height = 0;
    _width = 0;
    _codec = _ffmpeg->videoEncoder("");
    _language = new FFLanguage("");
    _colorPrimaries = _ffmpeg->colorPrimary("");
    _colorTRC = _ffmpeg->colorTRC("");
    _colorSpace = _ffmpeg->colorSpace("");
    _colorRange = _ffmpeg->colorRange("");
    _premultipliedAlpha = true;
}

VideoInfo::VideoInfo(QJsonObject obj, FFmpeg *ffmpeg, QObject *parent) : QObject(parent)
{
    _ffmpeg = ffmpeg;
    _language = nullptr;
    _id = -1;
    setQuality( obj.value("quality").toInt(), true);
    setProfile( obj.value("profile").toObject(), true);
    setLevel( obj.value("level").toString(), true);
    setPixAspect( obj.value("pixAspect").toDouble(), true);
    setPixFormat( obj.value("pixFormat").toObject(), true);
    if (!obj.value("premultipliedAlpha").isUndefined()) setPremultipliedAlpha( obj.value("premultipliedAlpha").toBool(), true);
    else _premultipliedAlpha = true;
    setBitrate( obj.value("bitrate").toInt(), true);
    setFramerate( obj.value("framerate").toDouble(), true);
    setHeight( obj.value("height").toInt(), true);
    setWidth( obj.value("width").toInt(), true);
    setCodec( obj.value("codec").toObject(), true);
    setLanguage( obj.value("language").toObject().value("name").toString(), true);
    setColorPrimaries( obj.value("colorPrimaries").toString(), true);
    setColorTRC( obj.value("colorTRC").toString(), true);
    setColorSpace( obj.value("colorSpace").toString(), true);
    setColorRange( obj.value("colorRange").toString(), true);
}

void VideoInfo::copyFrom(VideoInfo *other, bool silent)
{
    _id = other->id();
    _quality = other->quality();
    _profile = other->profile();
    _level = other->level();
    _pixAspect = other->pixAspect();
    _pixFormat = other->pixFormat();
    _bitrate = other->bitrate();
    _framerate = other->framerate();
    _height = other->height();
    _width = other->width();
    _codec = other->codec();
    delete _language;
    _language = other->language();
    _colorPrimaries = other->colorPrimaries();
    _colorTRC = other->colorTRC();
    _colorSpace = other->colorSpace();
    _colorRange = other->colorRange();
    _premultipliedAlpha = other->premultipliedAlpha();

    if(!silent) emit changed();
}

bool VideoInfo::isCopy()
{
    return _codec->name() == "copy";
}

QJsonObject VideoInfo::toJson()
{
    QJsonObject vStream;
    vStream.insert("quality", _quality);
    vStream.insert("profile", _profile->toJson() );
    vStream.insert("level", _level);
    vStream.insert("pixAspect", _pixAspect);
    vStream.insert("pixFormat", _pixFormat->toJson() );
    vStream.insert("premultipliedAlpha", _premultipliedAlpha);
    vStream.insert("bitrate", _bitrate);
    vStream.insert("framerate", _framerate);
    vStream.insert("height", _height);
    vStream.insert("width", _width);
    vStream.insert("codec", _codec->toJson() );
    vStream.insert("language", _language->toJson());
    vStream.insert("colorPrimaries", _colorPrimaries->toJson());
    vStream.insert("colorTRC", _colorTRC->toJson());
    vStream.insert("colorSpace", _colorSpace->toJson());
    vStream.insert("colorRange", _colorRange->toJson());

    return vStream;
}

int VideoInfo::quality() const
{
    return _quality;
}

void VideoInfo::setQuality(int quality, bool silent)
{
    _quality = quality;
    if(!silent) emit changed();
}

FFProfile *VideoInfo::profile() const
{
    return _profile;
}

void VideoInfo::setProfile(FFProfile *profile, bool silent)
{
    _profile = profile;
    if(!silent) emit changed();
}

void VideoInfo::setProfile(QString name, bool silent)
{
    setProfile( _ffmpeg->profile( name ), silent );
}

void VideoInfo::setProfile(QJsonObject obj, bool silent)
{
    setProfile( obj.value("name").toString(), silent );
}

float VideoInfo::aspect() const
{
    if (_height == 0) return 0;
    float aspect = float(_width) / float(_height);
    aspect = aspect * _pixAspect;
    return aspect;
}

float VideoInfo::pixAspect() const
{
    return _pixAspect;
}

void VideoInfo::setPixAspect(float pixAspect, bool silent)
{
    _pixAspect = pixAspect;
    if(!silent) emit changed();
}

FFPixFormat *VideoInfo::pixFormat() const
{
    return _pixFormat;
}

void VideoInfo::setPixFormat(FFPixFormat *pixFormat, bool silent)
{
    _pixFormat = pixFormat;
    if(!silent) emit changed();
}

void VideoInfo::setPixFormat(QString name, bool silent)
{
    setPixFormat( _ffmpeg->pixFormat( name ), silent );
}

void VideoInfo::setPixFormat(QJsonObject obj, bool silent)
{
    setPixFormat( obj.value("name").toString(), silent );
}

FFPixFormat *VideoInfo::defaultPixFormat() const
{
    return _codec->defaultPixFormat();
}

qint64 VideoInfo::bitrate() const
{
    return _bitrate;
}

void VideoInfo::setBitrate(const qint64 &bitrate, bool silent)
{
    _bitrate = bitrate;
    if(!silent) emit changed();
}

double VideoInfo::framerate() const
{
    return _framerate;
}

void VideoInfo::setFramerate(double framerate, bool silent)
{
    _framerate = framerate;
    if(!silent) emit changed();
}

int VideoInfo::height() const
{
    return _height;
}

void VideoInfo::setHeight(int height, bool silent)
{
    _height = height;
    if(!silent) emit changed();
}

int VideoInfo::width() const
{
    return _width;
}

void VideoInfo::setWidth(int width, bool silent)
{
    _width = width;
    if(!silent) emit changed();
}

FFCodec *VideoInfo::codec() const
{
    return _codec;
}

FFLanguage *VideoInfo::language() const
{
    return _language;
}

void VideoInfo::setLanguage(const QString &languageId, bool silent)
{
    delete _language;
    _language = new FFLanguage( languageId );
    if(!silent) emit changed();
}

void VideoInfo::setColorPrimaries(QString primaries, bool silent)
{
    setColorPrimaries( _ffmpeg->colorPrimary(primaries), silent );
}

void VideoInfo::setColorTRC(QString tRC, bool silent)
{
    setColorTRC( _ffmpeg->colorTRC(tRC), silent );
}

void VideoInfo::setColorSpace(QString space, bool silent)
{
    setColorSpace( _ffmpeg->colorSpace(space), silent);
}

void VideoInfo::setColorRange(QString range, bool silent)
{
    setColorRange( _ffmpeg->colorRange(range), silent);
}

void VideoInfo::setColorPrimaries(FFBaseObject *primaries, bool silent)
{
    _colorPrimaries = primaries;
    if(!silent) emit changed();
}

void VideoInfo::setColorTRC(FFBaseObject *tRC, bool silent)
{
    _colorTRC = tRC;
    if(!silent) emit changed();
}

void VideoInfo::setColorSpace(FFBaseObject *space, bool silent)
{
    _colorSpace = space;
    if(!silent) emit changed();
}

void VideoInfo::setColorRange(FFBaseObject *range, bool silent)
{
    _colorRange = range;
    if(!silent) emit changed();
}

void VideoInfo::setColorProfile(FFColorProfile *colorProfile, bool silent)
{
    _colorPrimaries = colorProfile->primaries();
    _colorTRC = colorProfile->trc();
    _colorSpace = colorProfile->space();
    _colorRange = colorProfile->range();
    if(!silent) emit changed();
}

void VideoInfo::setColorProfile(QString profile, bool silent)
{
    setColorProfile( _ffmpeg->colorProfile(profile), silent );
}

bool VideoInfo::setAlpha(bool alpha, bool silent)
{
    //select a pixfmt which has an alpha, the closest to the current one (or default)
    FFPixFormat *pf = _pixFormat;
    if ( pf->name() == "" ) pf = _codec->defaultPixFormat();
    if ( pf->name() == "" && _codec->pixFormats().count() > 0) pf = _codec->pixFormats()[0];

    _pixFormat = _codec->pixFormatWithAlpha(pf, alpha);

    if(!silent) emit changed();
    return true;
}

bool VideoInfo::hasAlpha()
{
    FFPixFormat *pf = _pixFormat;
    if (pf->name() == "") pf = _codec->defaultPixFormat();

    return pf->hasAlpha();
}

bool VideoInfo::canHaveAlpha()
{
    foreach (FFPixFormat *pf, _codec->pixFormats())
    {
        if (pf->hasAlpha()) return true;
    }

    return false;
}

int VideoInfo::id() const
{
    return _id;
}

void VideoInfo::setId(int id, bool silent)
{
    _id = id;
    if(!silent) emit changed();
}

QString VideoInfo::level() const
{
    return _level;
}

void VideoInfo::setLevel(const QString &level, bool silent)
{
    _level = level;
    if(!silent) emit changed();
}

bool VideoInfo::premultipliedAlpha() const
{
    return _premultipliedAlpha;
}

void VideoInfo::setPremultipliedAlpha(bool premultipliedAlpha, bool silent)
{
    _premultipliedAlpha = premultipliedAlpha;
    if(!silent) emit changed();
}

FFBaseObject *VideoInfo::colorPrimaries()
{
    return _colorPrimaries;
}

FFBaseObject *VideoInfo::colorTRC()
{
    return _colorTRC;
}

FFBaseObject *VideoInfo::colorSpace()
{
    return _colorSpace;
}

FFBaseObject *VideoInfo::colorRange()
{
    return _colorRange;
}

void VideoInfo::setCodec(FFCodec *codec, bool silent)
{
    _codec = codec;
    if(!silent) emit changed();
}

void VideoInfo::setCodec(QString name, bool silent)
{
    setCodec( _ffmpeg->videoEncoder(name), silent);
}

void VideoInfo::setCodec(QJsonObject obj, bool silent)
{
    setCodec( obj.value("name").toString(), silent );
}
