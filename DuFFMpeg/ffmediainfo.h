#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <QObject>

#include <QRegularExpression>

class FFMediaInfo : public QObject
{
    Q_OBJECT
public:
    explicit FFMediaInfo(QString ffmpeg = "",QObject *parent = nullptr);

    /**
     * @brief The unit used for sizes and bitrates
     */
    enum BitrateUnit { Bits, Kbps, Mbps};
    Q_ENUM(BitrateUnit)

    /**
     * @brief The unit used for sizes and bitrates
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
    void setVideoCodec(QString codec);
    void setAudioCodec(QString codec);
    void setVideoBitrate(double bitrate, BitrateUnit unit = Bits);
    void setAudioBitrate(double bitrate, BitrateUnit unit = Bits);
    void setSize(double s, SizeUnit unit = Bytes);
    void setFFmpegOptions(QStringList options);
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
    QString getVideoCodec();
    QString getAudioCodec();
    double getAudioBitrate(BitrateUnit unit = Bits);
    double getVideoBitrate(BitrateUnit unit = Bits);
    double getSize(SizeUnit unit = Bytes);
signals:

public slots:

private:
    QStringList container;
    int videoWidth;
    int videoHeight;
    double videoFramerate;
    int audioSamplingRate;
    double duration;
    QString ffmpegOutput;
    QString fileName;
    QString videoCodec;
    QString audioCodec;
    int audioBitrate;
    int videoBitrate;
    int size;
    bool imageSequence;
    QStringList ffmpegOptions;
};

#endif // MEDIAINFO_H
