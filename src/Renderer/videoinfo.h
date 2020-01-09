#ifndef VIDEOINFO_H
#define VIDEOINFO_H

#include <QObject>

#include "FFmpeg/ffpixformat.h"
#include "FFmpeg/ffcodec.h"

class VideoInfo : public QObject
{
    Q_OBJECT
public:
    explicit VideoInfo(QObject *parent = nullptr);

    int quality() const;
    void setQuality(int quality);

    int profile() const;
    void setProfile(int profile);

    float vaspect() const;
    void setVaspect(float vaspect);

    float pixAspect() const;
    void setPixAspect(float pixAspect);

    FFPixFormat *pixFormat() const;
    void setPixFormat(FFPixFormat *pixFormat);

    qint64 bitrate() const;
    void setBitrate(const qint64 &bitrate);

    double framerate() const;
    void setFramerate(double framerate);

    int height() const;
    void setHeight(int height);

    int width() const;
    void setWidth(int width);

    FFCodec *codec() const;

    QString language() const;
    void setLanguage(const QString &language);

signals:

private:
    int _quality;
    int _profile;
    float _vaspect;
    float _pixAspect;
    FFPixFormat *_pixFormat;
    qint64 _bitrate;
    double _framerate;
    int _height;
    int _width;
    FFCodec *_codec;
    QString _language;
};

#endif // VIDEOINFO_H
