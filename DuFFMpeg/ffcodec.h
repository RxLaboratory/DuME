#ifndef FFMPEGCODEC_H
#define FFMPEGCODEC_H

#include <QObject>

#include <QString>

class FFCodec : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief FFmpegCodec Constructs a codec instance for FFmpeg
     * @param n The internal name used by FFmpeg
     * @param prettyN The pretty name used for user interaction
     * @pqrqm v Is this a video codec (false if audio)
     * @param e Is the codec an encoder
     * @param d Is the codec a decoder
     */
    FFCodec(QString name = "", QString prettyName = "", bool video = true, bool encode = true, bool decode = true, bool lossy = true, bool lossless = true, bool iframe = false,QObject *parent = nullptr);
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
    void setDecoder(bool decoder);
    void setEncoder(bool encoder);
    void setAudio(bool audio);
    void setVideo(bool video);
    void setLossy(bool lossy);
    void setLossless(bool lossless);
    void setIframe(bool iframe);

private:
    QString _name;
    QString _prettyName;
    bool _decoder;
    bool _encoder;
    bool _audio;
    bool _video;
    bool _lossy;
    bool _lossless;
    bool _iframe;

protected:

};

#endif // FFMPEGCODEC_H
