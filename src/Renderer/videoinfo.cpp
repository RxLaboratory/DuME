#include "videoinfo.h"

VideoInfo::VideoInfo(QObject *parent) : QObject(parent)
{
    reInit();
}

void VideoInfo::reInit()
{
    _quality = -1;
    _profile = -1;
    _aspect = 0.0;
    _pixAspect = 1;
    _pixFormat = nullptr;
    _bitrate = 0;
    _framerate = 0.0;
    _height = 0;
    _width = 0;
    _codec = nullptr;
    _language = "";
}

int VideoInfo::quality() const
{
    return _quality;
}

void VideoInfo::setQuality(int quality)
{
    _quality = quality;
}

int VideoInfo::profile() const
{
    return _profile;
}

void VideoInfo::setProfile(int profile)
{
    _profile = profile;
}

float VideoInfo::aspect() const
{
    return _aspect;
}

void VideoInfo::setAspect(float vaspect)
{
    _aspect = vaspect;
}

float VideoInfo::pixAspect() const
{
    return _pixAspect;
}

void VideoInfo::setPixAspect(float pixAspect)
{
    _pixAspect = pixAspect;
}

FFPixFormat *VideoInfo::pixFormat() const
{
    return _pixFormat;
}

void VideoInfo::setPixFormat(FFPixFormat *pixFormat)
{
    _pixFormat = pixFormat;
}

qint64 VideoInfo::bitrate() const
{
    return _bitrate;
}

void VideoInfo::setBitrate(const qint64 &bitrate)
{
    _bitrate = bitrate;
}

double VideoInfo::framerate() const
{
    return _framerate;
}

void VideoInfo::setFramerate(double framerate)
{
    _framerate = framerate;
}

int VideoInfo::height() const
{
    return _height;
}

void VideoInfo::setHeight(int height)
{
    _height = height;
}

int VideoInfo::width() const
{
    return _width;
}

void VideoInfo::setWidth(int width)
{
    _width = width;
}

FFCodec *VideoInfo::codec() const
{
    return _codec;
}

QString VideoInfo::language() const
{
    return _language;
}

void VideoInfo::setLanguage(const QString &language)
{
    _language = language;
}

void VideoInfo::setCodec(FFCodec *codec)
{
    _codec = codec;
}
