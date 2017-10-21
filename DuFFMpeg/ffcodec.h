#ifndef FFMPEGCODEC_H
#define FFMPEGCODEC_H

#include "ffobject.h"

#include <QString>

class FFCodec : public FFObject
{
    Q_OBJECT
public:

    /**
     * @brief The type of media handled by the codec
     */
    enum Ability { Audio = 1 << 0,
                   Video = 1 << 1,
                   Subtitle = 1 << 2,
                   Encoder = 1 << 3,
                   Decoder = 1 << 4,
                   Lossy = 1 << 5,
                   Lossless = 1 << 6,
                   IFrame = 1 << 7
                 };
    Q_DECLARE_FLAGS(Abilities, Ability)

    /**
     * @brief FFmpegCodec Constructs a codec instance for FFmpeg
     * @param name The internal name used by FFmpeg
     */
    FFCodec(QObject *parent = nullptr);

    /**
     * @brief FFmpegCodec Constructs a codec instance for FFmpeg
     * @param name The internal name used by FFmpeg
     * @param prettyName The pretty name used for user interaction
     */
    FFCodec(QString name, QString prettyName = "", QObject *parent = nullptr);

    /**
     * @brief FFmpegCodec Constructs a codec instance for FFmpeg
     * @param name The internal name used by FFmpeg
     * @param prettyName The pretty name used for user interaction
     * @param abilities The abilities of the codec
     */
    FFCodec(QString name, QString prettyName, Abilities abilities, QObject *parent = nullptr);

    /**
     * @brief name Gets the internal name
     * @return The internal name
     */
    const QString name();
    /**
     * @brief prettyName Gets the pretty name
     * @return The pretty name
     */
    const QString prettyName();
    /**
     * @brief isVideo Is this a video codec
     * @return
     */
    bool isVideo() const;
    /**
     * @brief isAudio Is this an audio codec
     * @return
     */
    bool isAudio() const;
    /**
     * @brief isEncoder Can this codec be used for encoding
     * @return The encoding ability
     */
    bool isEncoder() const;
    /**
     * @brief isDecoder Can this codec be used for decoding
     * @return The decoding ability
     */
    bool isDecoder() const;
    /**
     * @brief isLossy Does this codec use lossy compression
     * @return Lossy compression ability
     */
    bool isLossy() const;
    /**
     * @brief isLossless Does this codec use lossless compression
     * @return Lossless compression ability
     */
    bool isLossless() const;
    /**
     * @brief isIframe Is this codec intra-frame
     * @return I-Frame ability
     */
    bool isIframe() const;

    void setName(const QString &name);
    void setPrettyName(const QString &prettyName);
    void setDecoder(bool decoder = true);
    void setEncoder(bool encoder = true);
    void setAudio(bool audio = true);
    void setVideo(bool video = true);
    void setLossy(bool lossy = true);
    void setLossless(bool lossless = true);
    void setIframe(bool iframe = true);
    void setAbilities(const Abilities &abilities);




private:
    QString _name;
    QString _prettyName;
    Abilities _abilities;

protected:

};

Q_DECLARE_OPERATORS_FOR_FLAGS(FFCodec::Abilities)

#endif // FFMPEGCODEC_H
