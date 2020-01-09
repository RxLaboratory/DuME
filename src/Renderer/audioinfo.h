#ifndef AUDIOINFO_H
#define AUDIOINFO_H

#include <QObject>

#include "FFmpeg/ffcodec.h"

class AudioInfo : public QObject
{
    Q_OBJECT
public:
    explicit AudioInfo(QObject *parent = nullptr);
    void reInit();

    int samplingRate() const;
    void setSamplingRate(int samplingRate);

    QString channels() const;
    void setChannels(const QString &channels);

    qint64 bitrate() const;
    void setBitrate(const qint64 &bitrate);

    FFCodec *codec() const;
    void setCodec(FFCodec *codec);

    QString language() const;
    void setLanguage(const QString &language);

signals:

private:
    int _samplingRate;
    QString _channels;
    qint64 _bitrate;
    FFCodec *_codec;
    QString _language;
};

#endif // AUDIOINFO_H
