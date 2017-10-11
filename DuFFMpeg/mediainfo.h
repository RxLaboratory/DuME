#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <QObject>

#include <QRegularExpression>

class MediaInfo : public QObject
{
    Q_OBJECT
public:
    explicit MediaInfo(QString ffmpeg,QObject *parent = nullptr);
    void setContainer(QStringList c);
    void setVideoWidth(int w);
    void setVideoHeight(int h);
    void setVideoFramerate(double fps);
    void setAudioSamplingRate(int s);
    void setDuration(double d);
    void setFileName(QString f);
    QStringList getContainer();
    int getVideoWidth();
    int getVideoHeight();
    double getVideoFramerate();
    int getAudioSamplingRate();
    double getDuration();
    QString getFfmpegOutput();
    QString getFileName();
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
};

#endif // MEDIAINFO_H
