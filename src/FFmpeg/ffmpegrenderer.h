#ifndef FFMPEGRENDERER_H
#define FFMPEGRENDERER_H

#include "Renderer/abstractrenderer.h"

#include <QObject>

class FFmpegRenderer : public AbstractRenderer
{
public:
    /**
     * @brief Gets the unique instance of FFmpegRenderer, constructing it if it's the first call.
     * @return
     */
    static FFmpegRenderer *instance();

    FFColorProfile *workingColorProfile() const;
    void setWorkingColorProfile(FFColorProfile *workingColorProfile);

protected:
    /**
     * @brief The unique FFmpegRenderer instance
     */
    static FFmpegRenderer *_instance;
    /**
     * @brief re-implemented from AbstractRenderer to interpret ffmpeg output
     * @param output
     */
    void readyRead(QString output);

private:
    // ======= OBJECTS =========

    // The FFmpeg instance
    FFmpeg *_ffmpeg;

    // Arguments of the FFmpeg command
    QStringList _inputArgs;
    QStringList _outputArgs;

    // Current characteristics
    double _jobFramerate;
    double _jobDuration;
    double _speedMultiplicator;
    FFColorItem *_inputTrc;
    FFColorItem *_inputPrimaries;
    FFColorProfile *_workingColorProfile;

    // ======= SINGLETON CONSTRUCTOR =======

    /**
     * @brief FFmpegRenderer is a singleton, the constructor is private
     * @param parent
     */
    FFmpegRenderer(QObject *parent = nullptr);

    // ======= METHODS =========

    /**
     * @brief Launches the current job, parsing MediaInfo to ffmpeg CLI arguments
     * @return
     */
    bool launchJob();
    /**
     * @brief Initializes arguments
     */
    void initJob();
    /**
     * @brief Prepares the input and gets its arguments
     * @param inputMedia The media
     */
    void setupInput(MediaInfo *inputMedia);
    /**
     * @brief Builds the custom ffmpeg arguments for the given media
     * @param media The media
     * @return The arguments
     */
    QStringList getFFmpegCustomOptions(MediaInfo *media);
    /**
     * @brief Builds the input sequence settings for the stream
     * @param sequence The stream
     * @return The arguments
     */
    QStringList getInputSequenceSettings(VideoInfo *sequence);
    /**
     * @brief Builds the video color metadata arguments
     * @param videoStream The stream
     * @return The arguments
     */
    QStringList getColorMetadata(VideoInfo *videoStream, FFColorProfile *defaultProfile, bool isInput = false);
    /**
     * @brief Gets the duration of the media, adjusted with in/out points
     * @param media The media
     * @return The duration
     */
    double getMediaDuration(MediaInfo *media);
    /**
     * @brief Builds the time range (for inputs)
     * @param media The media
     * @return The arguments
     */
    QStringList getTimeRange(MediaInfo *media);
    /**
     * @brief Gets the filename to use in FFmpeg commands
     * @param media The media
     * @return The filename
     */
    QString getFileName(MediaInfo *media);
    /**
     * @brief Prepares the output and gets its arguments
     * @param outputMedia The media
     */
    void setupOutput(MediaInfo *outputMedia);
    /**
     * @brief Builds the mapping arguments
     * @param media The media
     * @return The arguments
     */
    QStringList getMaps(MediaInfo *media);
    /**
     * @brief Builds the muxer settings
     * @param media The media
     * @return The arguments
     */
    QStringList getMuxer(MediaInfo *media);
    /**
     * @brief Gets the codec name for a video stream
     * @param stream The stream
     * @return The name
     */
    FFCodec *getFFCodec(VideoInfo  *stream, FFCodec *defaultCodec);
    /**
     * @brief Gets the codec name for a audio stream
     * @param stream The stream
     * @return The name
     */
    FFCodec *getFFCodec(AudioInfo  *stream, FFCodec *defaultCodec);
    /**
     * @brief Builds the codec settings for the given video stream
     * @param stream The stream
     * @return The arguments
     */
    QStringList getCodec(VideoInfo *stream);
    /**
     * @brief Builds the codec settings for the given audio stream
     * @param stream The stream
     * @return The arguments
     */
    QStringList getCodec(AudioInfo *stream);
    /**
     * @brief Builds the bitrate settings
     * @param stream The stream
     * @param codec The codec used to encode the stream
     * @return The arguments
     */
    QStringList getBitRate(VideoInfo *stream, FFCodec *codec);
    /**
     * @brief Builds the bitrate settings
     * @param stream The stream
     * @param codec The codec used to encode the stream
     * @return The arguments
     */
    QStringList getBitRate(AudioInfo *stream);
    /**
     * @brief Builds the sampling options
     * @param stream The stream
     * @return The arguments
     */
    QStringList getSampling(AudioInfo *stream);
    /**
     * @brief Builds the sample format options
     * @param stream The stream
     * @return The arguments
     */
    QStringList getSampleFormat(AudioInfo *stream);
    /**
     * @brief Builds the framerate settings
     * @param stream The stream
     * @return The arguments
     */
    QStringList getFramerate(VideoInfo *stream);
    /**
     * @brief Builds looping options
     * @param media The media
     * @param codec The codec used for encoding this stream
     * @return The arguments
     */
    QStringList getLoop(MediaInfo *media, FFCodec *codec);
    /**
     * @brief Builds the codec settings for a video stream
     * @param stream The video stream
     * @param codec The codec used for encoding this stream
     * @param pixFormat The pixel format of the stream
     * @return The arguments
     */
    QStringList getCodecSettings(VideoInfo *stream, FFCodec *codec, FFPixFormat *pixFormat);
    /**
     * @brief Builds settings for the output sequence
     * @param stream The stream
     * @return The arguments
     */
    QStringList getOutputSequenceSettings(VideoInfo *stream);
    /**
     * @brief Gets the pixel format to use for this stream
     * @param media The media
     * @param stream The video stream
     * @param codec The codec used to encode this stream
     * @return The pixel format
     */
    FFPixFormat *getPixelFormat(MediaInfo *media, VideoInfo *stream, FFCodec *codec);
    /**
     * @brief Builds the pixel format settings
     * @param stream The video stream
     * @param pixFormat The pixel format of the stream
     * @return The arguments
     */
    QStringList getPixelFormatSettings(VideoInfo *stream, FFPixFormat *pixFormat);
    /**
     * @brief Builds the filter chain
     * @param stream The stream
     * @return The arguments
     */
    QStringList getFilters(MediaInfo *media, VideoInfo *stream);
    QString unpremultiplyFilter(VideoInfo *stream);
    QString deinterlaceFilter(VideoInfo *stream);
    QStringList motionFilters(VideoInfo *stream);
    QString cropFilter(VideoInfo *stream);
    QStringList customVideoFilters(MediaInfo *media);
    QStringList colorConversionFilters(FFColorItem *outputTrc, FFColorItem *outputPrimaries, FFColorItem *outputMatrix = nullptr, FFColorItem *outputRange = nullptr, FFColorItem *inputTrc = nullptr, FFColorItem *inputPrimaries = nullptr, FFColorItem *inputMatrix = nullptr, FFColorItem *inputRange = nullptr);
    QStringList colorConversionFilters(FFColorProfile *from, FFColorItem *outputTrc, FFColorItem *outputPrimaries, FFColorItem *outputMatrix = nullptr, FFColorItem *outputRange = nullptr);
    QStringList colorConversionFilters(FFColorProfile *outputProfile);
    QStringList colorConversionFilters(FFColorProfile *from, FFColorProfile *to);
    QStringList inputColorConversionFilters();
    QStringList applyLutFilters(VideoInfo *stream, FFColorProfile *colorProfile);
    QStringList resizeFilters(VideoInfo *stream);
    /**
     * @brief Checks if the output needs a color conversion depending on both input & output formats, and sets it up
     * @return
     */
    bool colorManagement();
    /**
     * @brief Checks if there's a need for color conversion of the input to apply an output LUT
     * @return The LUT input color profile
     */
    FFColorProfile *convertInputForLut();
    /**
     * @brief Constructs the LUT filter to add to the filterchain
     * @return The ffmpeg filter description
     */
    QString getLutFilter(FFLut *lut);
    /**
     * @brief Builds a filter
     * @param filterName The name of the filter
     * @param args The arguments of the filter
     * @return The filter
     */
    QString generateFilter(QString filterName, QStringList args);
};

#endif // FFMPEGRENDERER_H
