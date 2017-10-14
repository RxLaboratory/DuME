#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <QObject>

#include <QRegularExpression>

#include "ffcodec.h"

class FFMediaInfo : public QObject
{
    Q_OBJECT
public:
    explicit FFMediaInfo(QString ffmpeg = "",QObject *parent = nullptr);

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

    void setContainer(QStringList c);
    void setVideoWidth(int w);
    void setVideoHeight(int h);
    void setVideoFramerate(double fps);
    void setAudioSamplingRate(int s);
    void setDuration(double d);
    void setFileName(QString f);
    void setVideoCodec(FFCodec codec);
    void setAudioCodec(FFCodec codec);
    void setVideoBitrate(double bitrate, BitrateUnit unit = Bits);
    void setAudioBitrate(double bitrate, BitrateUnit unit = Bits);
    void setSize(double s, SizeUnit unit = Bytes);
    void setFFmpegOptions(QStringList options);
    void setVideo(bool v = true);
    void setAudio(bool a = true);
    void addFFmpegOption(QString option);
    void removeFFmpegOpstion(QString option);
    void clearFFmpegOptions();
    QStringList getContainer();
    int getVideoWidth();
    int getVideoHeight();
    double getVideoFramerate();
    int getAudioSamplingRate();
    double getDuration();
    QString getFfmpegOutput();
    QString getFileName();
    FFCodec getVideoCodec();
    FFCodec getAudioCodec();
    double getAudioBitrate(BitrateUnit unit = Bits);
    double getVideoBitrate(BitrateUnit unit = Bits);
    double getSize(SizeUnit unit = Bytes);
    QStringList getFFmpegOptions();
    bool hasVideo();
    bool hasAudio();
    bool isImageSequence();
signals:

public slots:

private:
    QStringList container;
    double duration;
    int size;
    bool video;
    bool audio;
    bool imageSequence;
    FFCodec videoCodec;
    int videoWidth;
    int videoHeight;
    double videoFramerate;
    int videoBitrate;
    FFCodec audioCodec;
    int audioSamplingRate;
    int audioBitrate;
    QString ffmpegOutput;
    QString fileName;  
    QStringList ffmpegOptions;

};

#endif // MEDIAINFO_H
