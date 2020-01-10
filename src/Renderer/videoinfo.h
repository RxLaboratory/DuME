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
    void reInit();

    int quality() const;
    void setQuality(int quality);

    QString profile() const;
    void setProfile(QString profile);

    float aspect() const;
    void setAspect(float aspect);

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
    void setCodec(FFCodec *codec);

    QString language() const;
    void setLanguage(const QString &language);

    bool hasAlpha();
    bool canHaveAlpha();

    int id() const;
    void setId(int id);

    QString level() const;
    void setLevel(const QString &level);

signals:

private:
    int _id;
    int _quality;
    QString _profile;
    QString _level;
    float _aspect;
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
