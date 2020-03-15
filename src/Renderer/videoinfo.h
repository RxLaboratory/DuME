#ifndef VIDEOINFO_H
#define VIDEOINFO_H

#include <QObject>

#include "global.h"

#include "FFmpeg/ffmpeg.h"
#include "FFmpeg/fflanguage.h"

class VideoInfo : public QObject
{
    Q_OBJECT
public:
    explicit VideoInfo(QObject *parent = nullptr);
    explicit VideoInfo(QJsonObject obj, QObject *parent = nullptr);
    void copyFrom(VideoInfo *other, bool silent = false);
    bool isCopy();
    QJsonObject toJson();
    QString getDescription();

    int quality() const;
    void setQuality(int quality, bool silent = false);

    int encodingSpeed() const;
    void setEncodingSpeed(int encodingSpeed, bool silent = false);

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

    void setColorPrimaries(QString primaries, bool silent = false);
    void setColorTRC(QString tRC, bool silent = false);
    void setColorSpace(QString space, bool silent = false);
    void setColorRange(QString range, bool silent = false);
    void setColorPrimaries(FFBaseObject *primaries, bool silent = false);
    void setColorTRC(FFBaseObject *tRC, bool silent = false);
    void setColorSpace(FFBaseObject *space, bool silent = false);
    void setColorRange(FFBaseObject *range, bool silent = false);
    void setColorProfile(FFColorProfile *colorProfile, bool silent = false);
    void setColorProfile(QString profile, bool silent = false);
    FFBaseObject *colorPrimaries();
    FFBaseObject *colorTRC();
    FFBaseObject *colorSpace();
    FFBaseObject *colorRange();

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

    FFBaseObject *tuning() const;
    void setTuning(FFBaseObject *tuning, bool silent = false);
    void setTuning( QString tuning, bool silent = false);
    void setTuning( QJsonObject tuning, bool silent = false);

    MediaUtils::BitrateType bitrateType() const;
    void setBitrateType(const MediaUtils::BitrateType &bitrateType, bool silent = false);
    void setBitrateType(QString bitrateType, bool silent = false);

signals:
    void changed();

private:
    /**
     * @brief _id UUID of this stream
     */
    int _id;
    /**
     * @brief _quality Quality of the video, influences bitrate. -1: auto, 0: worst, 100: best.
     * Supported codecs: h264
     */
    int _quality;
    /**
     * @brief _encodingSpeed Quality vs Encoding speed on supported codecs. -1: default, 0: best quality, 100: fastest.
     * Supported codecs: h264
     */
    int _encodingSpeed;
    /**
     * @brief _profile Codec/Video profile.
     * Supported codecs: h264, prores
     */
    FFProfile *_profile;
    /**
     * @brief _profile Codec level.
     * Supported codecs: h264
     */
    QString _level;
    /**
     * @brief _tuning Preset to fine-tune the encoding.
     * Supported codecs: h264
     */
    FFBaseObject *_tuning;
    /**
     * @brief _bitrateType VBR or CBR
     */
    MediaUtils::BitrateType _bitrateType;
    float _pixAspect;
    FFPixFormat *_pixFormat;
    qint64 _bitrate;
    double _framerate;
    int _height;
    int _width;
    FFCodec *_codec;
    FFLanguage *_language;
    FFBaseObject *_colorPrimaries;
    FFBaseObject *_colorTRC;
    FFBaseObject *_colorSpace;
    FFBaseObject *_colorRange;
    bool _premultipliedAlpha;
};

#endif // VIDEOINFO_H
