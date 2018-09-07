#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include "ffobject.h"

#include <QRegularExpression>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QDir>

#include "ffcodec.h"
#include "ffmuxer.h"

class FFMediaInfo : public FFObject
{
    Q_OBJECT
public:
    explicit FFMediaInfo(QString ffmpegOutput = "",QObject *parent = nullptr);

    /**
     * @brief The unit used for bitrates
     */
    enum BitrateUnit { Bits, Kbps, Mbps};
    Q_ENUM(BitrateUnit)

    /**
     * @brief The unit used for sizes
     */
    enum SizeUnit { Bytes, KB, MB};
    Q_ENUM(SizeUnit)

    //setters
    void setMuxer(FFMuxer *muxer);
    void updateInfo(QString ffmpegOutput);
    void setContainer(QStringList container);
    void setVideoWidth(int width);
    void setVideoHeight(int height);
    void setVideoFramerate(double fps);
    void setAudioSamplingRate(int sampling);
    void setDuration(double duration);
    void setFileName(QString fileName);
    void setVideoCodec(FFCodec *codec);
    void setAudioCodec(FFCodec *codec);
    void setVideoBitrate(double bitrate, BitrateUnit unit = Bits);
    void setAudioBitrate(double bitrate, BitrateUnit unit = Bits);
    void setSize(double size, SizeUnit unit = Bytes);
    void setFFmpegOptions(QList<QStringList> options);
    void setVideo(bool video = true);
    void setAudio(bool audio = true);
    void setVideoProfile(int profile);
    void setVideoQuality(int quality);
    void setLoop(int loop);
    void addFFmpegOption(QStringList option);
    void removeFFmpegOpstion(QString optionName);
    void clearFFmpegOptions();
    void setStartNumber(int startNumber);
    void setFrames(const QStringList &frames);
    void setPixFormat(FFPixFormat *pixFormat);
    void setPremultipliedAlpha(bool premultipliedAlpha);
    void setTrc(const QString &trc);
    void setAep(bool isAep);
    void setAepNumThreads(int aepNumThreads);
    void setAepCompName(const QString &aepCompName);
    //getters
    FFMuxer *muxer() const;
    int videoWidth();
    int videoHeight();
    double videoFramerate();
    int audioSamplingRate();
    double duration();
    QString ffmpegOutput();
    QString fileName();
    FFCodec *videoCodec();
    FFCodec *audioCodec();
    FFPixFormat *pixFormat();
    double audioBitrate(BitrateUnit unit = Bits);
    double videoBitrate(BitrateUnit unit = Bits);
    double size(SizeUnit unit = Bytes);
    QList<QStringList> ffmpegOptions();
    bool hasVideo();
    bool hasAudio();
    bool isImageSequence();
    QStringList extensions() const;
    int videoQuality() const;
    int videoProfile() const;
    int loop() const;
    int startNumber() const;
    QStringList frames() const;
    bool premultipliedAlpha() const;
    QString trc() const;
    bool isAep() const;
    QString aepCompName() const;
    int aepNumThreads() const;

    //utils
    QString exportToJson();
    void exportToJson(QString jsonPath);

signals:

public slots:

private:
    QStringList _extensions;
    FFMuxer *_muxer;
    double _duration;
    int _size;
    bool _video;
    bool _audio;
    bool _imageSequence;
    FFCodec *_videoCodec;
    int _videoWidth;
    int _videoHeight;
    double _videoFramerate;
    int _videoBitrate;
    FFPixFormat *_pixFormat;
    FFCodec *_audioCodec;
    int _audioSamplingRate;
    int _audioBitrate;
    QString _ffmpegOutput;
    QString _fileName;
    QStringList _frames;
    QList<QStringList> _ffmpegOptions;
    int _videoQuality;
    int _videoProfile;
    int _loop;
    int _startNumber;
    bool _premultipliedAlpha;
    QString _trc;
    bool _isAep;
    int _aepNumThreads;
    QString _aepCompName;

    void loadSequence();

};

#endif // MEDIAINFO_H
