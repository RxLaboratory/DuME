#include "ffmpegrenderer.h"

#include <QtDebug>

FFmpegRenderer::FFmpegRenderer(QString ffmpeg, QObject *parent) : AbstractRenderer(parent)
{
    setBinary( ffmpeg );
}

void FFmpegRenderer::readyRead(QString output)
{
    QRegularExpression reProgress("(?:frame= *(\\d+).*fps= *(\\d+).*)?size= *(?:(\\d+)kB)?.*time=(\\d\\d:\\d\\d:\\d\\d.\\d\\d).*bitrate= *(?:(\\d+).\\d+kbits)?.*speed= *(\\d+.\\d*)x");
    QRegularExpressionMatch match = reProgress.match(output);

    //if progress, update UI
    if (match.hasMatch())
    {
        QString frame = match.captured(1);
        QString size = match.captured(3);
        QString bitrate = match.captured(5);
        QString speed = match.captured(6);

         //size
        int sizeKB = size.toInt();

        //bitrate
        int bitrateKB = bitrate.toInt();

        //frame
        setCurrentFrame( frame.toInt(), sizeKB * 1024, bitrateKB * 1000, speed.toDouble() );

        emit progress();
    }

    emit console(output);
}
