#ifndef FFMPEGRENDERER_H
#define FFMPEGRENDERER_H

#include "abstractrenderer.h"

#include <QObject>

class FFmpegRenderer : public AbstractRenderer
{
public:
    FFmpegRenderer(QString ffmpeg, QObject *parent = nullptr);

private:
    //re-implemented from AbstractRenderer to interpret ffmpeg output
    void readyRead(QString output);
};

#endif // FFMPEGRENDERER_H
