#include "ffmpegrenderer.h"

FFmpegRenderer::FFmpegRenderer(QString ffmpeg, QObject *parent) : AbstractRenderer(parent)
{
    setBinaryFileName( ffmpeg );
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

        //frame
        setCurrentFrame( frame.toInt() );

        //size
        int sizeKB = size.toInt();
        _outputSize = sizeKB * 1024;

        //bitrate
        int bitrateKB = bitrate.toInt();
        _outputBitrate = bitrateKB * 1024 * 8;

        //speed
        _encodingSpeed = speed.toDouble();

        emit progress();
    }
}
