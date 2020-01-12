#ifndef AUDIOINFO_H
#define AUDIOINFO_H

#include <QObject>

#include "FFmpeg/ffmpeg.h"
#include "FFmpeg/fflanguage.h"

class AudioInfo : public QObject
{
    Q_OBJECT
public:
    explicit AudioInfo(FFmpeg *ffmpeg, QObject *parent = nullptr);
    explicit AudioInfo(QJsonObject obj, FFmpeg *ffmpeg, QObject *parent = nullptr);
    void copyFrom(AudioInfo *other, bool silent = false);
    bool isCopy();
    QJsonObject toJson();

    int samplingRate() const;
    void setSamplingRate(int samplingRate, bool silent = false);

    QString channels() const;
    void setChannels(const QString &channels, bool silent = false);

    qint64 bitrate() const;
    void setBitrate(const qint64 &bitrate, bool silent = false);

    FFCodec *codec() const;
    void setCodec(FFCodec *codec, bool silent = false);
    void setCodec(QString name, bool silent = false);
    void setCodec(QJsonObject obj, bool silent = false);

    FFLanguage *language() const;
    void setLanguage(const QString &languageId, bool silent = false);

    int id() const;
    void setId(int id, bool silent = false);

signals:
    void changed();
private:
    int _id;
    int _samplingRate;
    QString _channels;
    qint64 _bitrate;
    FFCodec *_codec;
    FFLanguage *_language;

    FFmpeg *_ffmpeg;
};

#endif // AUDIOINFO_H
