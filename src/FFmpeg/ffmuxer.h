#ifndef FFMUXER_H
#define FFMUXER_H

#include "ffbaseobject.h"
#include "ffcodec.h"

class FFMuxer : public FFBaseObject
{
    Q_OBJECT
public:

    /**
     * @brief The muxer type
     */
    enum Type { AudioOnly, VideoOnly, AudioVideo };
    Q_ENUM(Type)

    explicit FFMuxer(QString name, QString prettyName = "",QObject *parent = nullptr);
    explicit FFMuxer(QString name, QString prettyName,Type type, QObject *parent = nullptr);
    QJsonObject toJson();

    /**
     * @brief defaultVideoCodec Gets the default video codec for this muxer
     * @return The default codec or nullptr if this muxer does not have a default codec
     */
    FFCodec * defaultVideoCodec() const;
    void setDefaultVideoCodec(FFCodec *defaultVideoCodec);

    /**
     * @brief defaultVideoCodec Gets the default audio codec for this muxer
     * @return The default codec or nullptr if this muxer does not have a default codec
     */
    FFCodec * defaultAudioCodec() const;
    void setDefaultAudioCodec(FFCodec *defaultAudioCodec);

    bool isAudio();
    bool isVideo();
    bool isSequence();
    void setType(const Type &type);
    void setSequence(bool sequence = true);

    QStringList extensions() const;
    void setExtensions(const QStringList &extensions);


signals:

public slots:

private:
    FFCodec *_defaultVideoCodec;
    FFCodec *_defaultAudioCodec;
    Type _type;
    QStringList _extensions;
    bool _sequence;

    void checkType();
};

#endif // FFMUXER_H
