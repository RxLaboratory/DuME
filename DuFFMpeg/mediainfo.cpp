#include "mediainfo.h"

#include <QtDebug>

MediaInfo::MediaInfo(QString ffmpeg, QObject *parent) : QObject(parent)
{
    QStringList infos = ffmpeg.split("\n");

    bool input = false;

    //regexes to get infos
    QRegularExpression reInput("Input #\\d+, ([\\w+,]+) from '(.+)':");
    QRegularExpression reVideoStream("Stream #.+Video: .+, (\\d+)x(\\d+).+, (\\d{1,2}.?\\d{0,2}) fps");
    QRegularExpression reAudioStream("Stream #.+Audio: .+, (\\d{4,6}) Hz");

    foreach(QString info,infos)
    {
        //test input
        QRegularExpressionMatch match = reInput.match(info);
        if (match.hasMatch())
        {
            input = true;
            container = match.captured(1).split(",");
            fileName = match.captured(2);
            qDebug() << info;
        }

        if (!input) continue;

        ffmpegOutput = ffmpegOutput + "\n" + info;

        //TODO Needs to get duration

        //test video stream
        match = reVideoStream.match(info);
        if (match.hasMatch())
        {
            qDebug() << info;
            //set size
            videoWidth = match.captured(1).toInt();
            videoHeight = match.captured(2).toInt();
            videoFramerate = match.captured(3).toDouble();
            continue;
        }

        //test audio stream
        match = reAudioStream.match(info);
        if (match.hasMatch())
        {
            qDebug() << info;
            //set sampling rate
            audioSamplingRate = match.captured(1).toInt();
            continue;
        }
    }

}

void MediaInfo::setContainer(QStringList c)
{
    container = c;
}

void MediaInfo::setVideoWidth(int w)
{
    videoWidth = w;
}

void MediaInfo::setVideoHeight(int h)
{
    videoHeight = h;
}

void MediaInfo::setVideoFramerate(double fps)
{
    videoFramerate = fps;
}

void MediaInfo::setAudioSamplingRate(int s)
{
    audioSamplingRate = s;
}

void MediaInfo::setDuration(double d)
{
    duration = d;
}

void MediaInfo::setFileName(QString f)
{
    fileName = f;
}

QStringList MediaInfo::getContainer()
{
    return container;
}

int MediaInfo::getVideoWidth()
{
    return videoWidth;
}

int MediaInfo::getVideoHeight()
{
    return videoHeight;
}

double MediaInfo::getVideoFramerate()
{
    return videoFramerate;
}

int MediaInfo::getAudioSamplingRate()
{
    return audioSamplingRate;
}

double MediaInfo::getDuration()
{
    return duration;
}

QString MediaInfo::getFfmpegOutput()
{
    return ffmpegOutput;
}

QString MediaInfo::getFileName()
{
    return fileName;
}
