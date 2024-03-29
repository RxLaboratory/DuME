#ifndef VIDEOINFO_H
#define VIDEOINFO_H

#include <QObject>
#include <QMetaEnum>

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
    /**
     * @brief Generates a human readable description of the media
     * @param outputMedia If True, includes some information about the way the media will be generated
     * @return The description
     */
    QString getDescription(bool outputMedia = false );

    int quality() const;
    void setQuality(int quality, bool silent = false);

    int encodingSpeed() const;
    void setEncodingSpeed(int encodingSpeed, bool silent = false);

    FFBaseObject *profile() const;
    void setProfile(FFBaseObject *profile, bool silent = false);
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
    void setColorPrimaries(QJsonObject primaries, bool silent = false);
    void setColorTRC(QString tRC, bool silent = false);
    void setColorTRC(QJsonObject tRC, bool silent = false);
    void setColorSpace(QString space, bool silent = false);
    void setColorSpace(QJsonObject space, bool silent = false);
    void setColorRange(QString range, bool silent = false);
    void setColorRange(QJsonObject range, bool silent = false);
    void setColorPrimaries(FFColorItem *primaries, bool silent = false);
    void setColorTRC(FFColorItem *tRC, bool silent = false);
    void setColorSpace(FFColorItem *space, bool silent = false);
    void setColorRange(FFColorItem *range, bool silent = false);
    void setColorProfile(FFColorProfile *colorProfile, bool silent = false);
    void setColorProfile(QString profile, bool silent = false);
    FFColorItem *colorPrimaries();
    FFColorItem *colorTRC();
    FFColorItem *colorSpace();
    FFColorItem *colorRange();
    FFColorProfile *workingSpace() const;
    void setWorkingSpace(FFColorProfile *workingSpace, bool silent = false);
    void setWorkingSpace(QString workingSpace, bool silent = false);
    void setWorkingSpace(QJsonObject workingSpace, bool silent = false);

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

    void setCrop(int top, int bottom, int left, int right, bool silent = false);
    void setCrop(int width, int height, bool silent = false);

    int topCrop() const;
    int bottomCrop() const;
    int leftCrop() const;
    int rightCrop() const;
    int cropHeight() const;
    int cropWidth() const;

    bool cropUseSize() const;
    void setCropUseSize(bool cropUseSize, bool silent = false);

    FFLut *lut() const;
    void setLut(const QString &lut, bool silent = false);
    void setLut(const QJsonObject &lut, bool silent = false);
    void setLut(FFLut *lut, bool silent = false);
    bool applyLutOnOutputSpace() const;
    void setApplyLutOnOutputSpace(bool applyLutOnOutputSpace, bool silent = false);

    bool deinterlace() const;
    void setDeinterlace(bool deinterlace, bool silent = false);

    MediaUtils::DeinterlaceParity deinterlaceParity() const;
    void setDeinterlaceParity(const MediaUtils::DeinterlaceParity &deinterlaceParity, bool silent = false);

    bool intra() const;
    void setIntra(bool intra, bool silent = false);

    bool lossless() const;
    void setLossless(bool lossless, bool silent = false);

    float speed() const;
    void setSpeed(float speed, bool silent = false);

    MediaUtils::MotionInterpolationMode speedInterpolationMode() const;
    void setSpeedInterpolationMode(const MediaUtils::MotionInterpolationMode &speedInterpolationMode, bool silent = false);

    FFBaseObject *speedEstimationMode() const;
    void setSpeedEstimationMode(FFBaseObject *speedEstimationMode, bool silent = false);
    void setSpeedEstimationMode(QString speedEstimationMode, bool silent = false);
    void setSpeedEstimationMode(QJsonObject speedEstimationMode, bool silent = false);

    FFBaseObject *speedAlgorithm() const;
    void setSpeedAlgorithm(FFBaseObject *speedAlgorithm, bool silent = false);
    void setSpeedAlgorithm(QString speedAlgorithm, bool silent = false);
    void setSpeedAlgorithm(QJsonObject speedAlgorithm, bool silent = false);

    bool sceneDetection() const;
    void setSceneDetection(bool sceneDetection, bool silent = false);

    MediaUtils::ResizeMode resizeMode() const;
    void setResizeMode(const MediaUtils::ResizeMode &resizeMode, bool silent = false);

    FFBaseObject *resizeAlgorithm() const;
    void setResizeAlgorithm(FFBaseObject *resizeAlgorithm, bool silent = false);
    void setResizeAlgorithm(QString resizeAlgorithm, bool silent = false);
    void setResizeAlgorithm(QJsonObject resizeAlgorithm, bool silent = false);

    MediaUtils::ColorConversionMode colorConversionMode() const;
    void setColorConversionMode(MediaUtils::ColorConversionMode colorConversionMode, bool silent = false);
    void setColorConversionMode(QString colorConversionMode, bool silent = false);

    bool isSequence() const;
    void setSequence(bool isSequence, bool silent = false);

    int startNumber() const;
    void setStartNumber(int startNumber, bool silent = false);

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
    FFBaseObject *_profile;
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
    /**
     * @brief When resized, wether to letterbox or stretch the video
     */
    MediaUtils::ResizeMode _resizeMode;
    FFBaseObject *_resizeAlgorithm;
    FFCodec *_codec;
    FFLanguage *_language;
    FFColorItem *_colorPrimaries;
    FFColorItem *_colorTRC;
    FFColorItem *_colorSpace;
    FFColorItem *_colorRange;
    MediaUtils::ColorConversionMode _colorConversionMode;
    FFColorProfile *_workingSpace;
    bool _premultipliedAlpha;
    int _topCrop;
    int _bottomCrop;
    int _leftCrop;
    int _rightCrop;
    int _cropHeight;
    int _cropWidth;
    bool _cropUseSize;
    FFLut *_lut;
    bool _applyLutOnOutputSpace;
    bool _deinterlace;
    MediaUtils::DeinterlaceParity _deinterlaceParity;
    bool _intra;
    bool _lossless;
    /**
     * @brief _speed Multiplicator > 0.0: < 1.0 to slow down, > 1.0 to speed up.
     */
    float _speed;
    MediaUtils::MotionInterpolationMode _speedInterpolationMode;
    FFBaseObject *_speedEstimationMode;
    FFBaseObject *_speedAlgorithm;
    bool _sceneDetection;
    bool _isSequence;
    int _startNumber;
};

#endif // VIDEOINFO_H
