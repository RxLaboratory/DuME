#ifndef FFMUXER_H
#define FFMUXER_H

#include "ffbaseobject.h"
#include "ffcodec.h"
#include "ffcolorprofile.h"

class FFMuxer : public FFBaseObject
{
    Q_OBJECT
public:

    /**
     * @brief The muxer type
     */
    enum Type{ Audio = 1 << 0,
               Video = 1 << 1,
               Sequence = 1 << 2,
               Subtitles = 1 << 3
             };
    Q_DECLARE_FLAGS(Types,Type)

    explicit FFMuxer(QString name, QString prettyName = "",QObject *parent = nullptr);
    explicit FFMuxer(QString name, QString prettyName, Types types, QObject *parent = nullptr);
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
    bool isDecodeOnly() const;
    void setAudio(bool audio = true);
    void setVideo(bool video = true);
    void setSequence(bool sequence = true);
    void setDecodeOnly(bool decodeOnly);

    QStringList extensions() const;
    void setExtensions(const QStringList &extensions);

    static FFMuxer *getDefault(QObject *parent = nullptr);

    QString defaultColorProfile() const;
signals:

public slots:

private:
    FFCodec *_defaultVideoCodec;
    FFCodec *_defaultAudioCodec;
    QString _defaultColorProfile;
    Types _types;
    QStringList _extensions;
    bool _decodeOnly;
    void init();
};

#endif // FFMUXER_H
