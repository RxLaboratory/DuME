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
