#ifndef VIDEOINFO_H
#define VIDEOINFO_H

#include <QObject>

#include "FFmpeg/ffmpeg.h"
#include "FFmpeg/fflanguage.h"

class VideoInfo : public QObject
{
    Q_OBJECT
public:
    explicit VideoInfo(FFmpeg *ffmpeg, QObject *parent = nullptr);
    explicit VideoInfo(QJsonObject obj, FFmpeg *ffmpeg, QObject *parent = nullptr);
    void reInit(bool silent = false);
    void copyFrom(VideoInfo *other, bool silent = false);
    bool isCopy();
    QJsonObject toJson();

    int quality() const;
    void setQuality(int quality, bool silent = false);

    FFProfile *profile() const;
    void setProfile(FFProfile *profile, bool silent = false);
    void setProfile(QString name, bool silent = false);
    void setProfile(QJsonObject obj, bool silent =  false);

    float aspect() const;

    float pixAspect() const;
    void setPixAspect(float pixAspect, bool silent = false);

    FFPixFormat *pixFormat() const;
    void setPixFormat(FFPixFormat *pixFormat, bool silent = false);
    void setPixFormat(QString name, bool silent = false);
    void setPixFormat(QJsonObject obj, bool silent =  false);
    FFPixFormat *defaultPixFormat() const;

    qint64 bitrate() const;
    void setBitrate(const qint64 &bitrate, bool silent = false);

    double framerate() const;
    void setFramerate(double framerate, bool silent = false);

    int height() const;
    void setHeight(int height, bool silent = false);

    int width() const;
    void setWidth(int width, bool silent = false);

    FFCodec *codec() const;
    void setCodec(FFCodec *codec, bool silent = false);
    void setCodec(QString name, bool silent = false);
    void setCodec(QJsonObject obj, bool silent =  false);

    FFLanguage *language() const;
    void setLanguage(const QString &languageId, bool silent = false);

    void setColorPrimaries(const QString &primaries, bool silent = false);
    void setColorTRC(const QString &tRC, bool silent = false);
    void setColorSpace(const QString &space, bool silent = false);
    void setColorRange(const QString &range, bool silent = false);
    QString colorPrimaries() const;
    QString colorTRC() const;
    QString colorSpace() const;
    QString colorRange() const;

    /**
     * @brief setAlpha Tries to set a pixel format which has alpha (or not)
     * @param alpha
     * @param silent
     * @return True if the pixel format could be set, false if not possible (no codec set)
     */
    bool setAlpha(bool alpha, bool silent = false);
    bool hasAlpha();
    bool canHaveAlpha();

    int id() const;
    void setId(int id, bool silent = false);

    QString level() const;
    void setLevel(const QString &level, bool silent = false);

    bool premultipliedAlpha() const;
    void setPremultipliedAlpha(bool premultipliedAlpha, bool silent = false);

signals:
    void changed();

private:
    int _id;
    int _quality;
    FFProfile *_profile;
    QString _level;
    float _pixAspect;
    FFPixFormat *_pixFormat;
    qint64 _bitrate;
    double _framerate;
    int _height;
    int _width;
    FFCodec *_codec;
    FFLanguage *_language;
    QString _colorPrimaries;
    QString _colorTRC;
    QString _colorSpace;
    QString _colorRange;
    bool _premultipliedAlpha;

    FFmpeg *_ffmpeg;
};

#endif // VIDEOINFO_H
