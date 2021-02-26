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

private:
    /**
     * @brief FFmpegRenderer is a singleton, the constructor is private
     * @param parent
     */
    FFmpegRenderer(QObject *parent = nullptr);
    /**
     * @brief Launches the current job, parsing MediaInfo to ffmpeg CLI arguments
     * @return
     */
    bool launchJob();
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
    QString getLutFilter(VideoInfo *stream);

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
};

#endif // FFMPEGRENDERER_H
