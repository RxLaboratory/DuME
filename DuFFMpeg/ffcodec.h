#ifndef FFMPEGCODEC_H
#define FFMPEGCODEC_H

#include <QString>

class FFCodec
{
public:
    /**
     * @brief FFmpegCodec Constructs a codec instance for FFmpeg
     * @param n The internal name used by FFmpeg
     * @param prettyN The pretty name used for user interaction
     * @pqrqm v Is this a video codec (false if audio)
     * @param e Is the codec an encoder
     * @param d Is the codec a decoder
     */
    FFCodec(QString n = "", QString prettyN = "", bool vid = true, bool encode = true, bool decode = true);
    /**
     * @brief getName Gets the internal name
     * @return The internal name
     */
    QString getName();
    /**
     * @brief getPrettyName Gets the pretty name
     * @return The pretty name
     */
    QString getPrettyName();
    /**
     * @brief isVideo Is this a video codec
     * @return
     */
    bool isVideo();
    /**
     * @brief isAudio Is this an audio codec
     * @return
     */
    bool isAudio();
    /**
     * @brief isEncoder Can this codec be used for encoding
     * @return The encoding ability
     */
    bool isEncoder();
    /**
     * @brief isDecoderCan this codec be used for decoding
     * @return The decoding ability
     */
    bool isDecoder();

private:
    QString name;
    QString prettyName;
    bool decoder;
    bool encoder;
    bool audio;
    bool video;
};

#endif // FFMPEGCODEC_H
