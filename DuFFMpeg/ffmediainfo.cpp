#include "ffmediainfo.h"

#include <QtDebug>

FFMediaInfo::FFMediaInfo(QString ffmpeg, QObject *parent) : QObject(parent)
{
    container = QStringList();
    fileName = "";
    duration = 0.0;
    videoWidth = 0;
    videoHeight = 0;
    videoFramerate = 0.0;
    audioSamplingRate = 0;


    QStringList infos = ffmpeg.split("\n");

    bool input = false;

    //regexes to get infos
    QRegularExpression reInput("Input #\\d+, ([\\w+,]+) from '(.+)':");
    QRegularExpression reVideoStream("Stream #.+Video: .+, (\\d+)x(\\d+).+, (\\d{1,2}.?\\d{0,2}) fps");
    QRegularExpression reAudioStream("Stream #.+Audio: .+, (\\d{4,6}) Hz");
    QRegularExpression reDuration("  Duration: (\\d\\d):(\\d\\d):(\\d\\d.\\d\\d), ");

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

        //test duration
        match = reDuration.match(info);
        if (match.hasMatch())
        {
            qDebug() << info;
            //set duration
            double h = match.captured(1).toDouble();
            double m = match.captured(2).toDouble();
            double s = match.captured(3).toDouble();
            duration = h*60*60+m*60+s;
            qDebug() << "Duration" << duration;
        }

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

void FFMediaInfo::setContainer(QStringList c)
{
    container = c;
}

void FFMediaInfo::setVideoWidth(int w)
{
    videoWidth = w;
}

void FFMediaInfo::setVideoHeight(int h)
{
    videoHeight = h;
}

void FFMediaInfo::setVideoFramerate(double fps)
{
    videoFramerate = fps;
}

void FFMediaInfo::setAudioSamplingRate(int s)
{
    audioSamplingRate = s;
}

void FFMediaInfo::setDuration(double d)
{
    duration = d;
}

void FFMediaInfo::setFileName(QString f)
{
    fileName = f;
}

QStringList FFMediaInfo::getContainer()
{
    return container;
}

int FFMediaInfo::getVideoWidth()
{
    return videoWidth;
}

int FFMediaInfo::getVideoHeight()
{
    return videoHeight;
}

double FFMediaInfo::getVideoFramerate()
{
    return videoFramerate;
}

int FFMediaInfo::getAudioSamplingRate()
{
    return audioSamplingRate;
}

double FFMediaInfo::getDuration()
{
    return duration;
}

QString FFMediaInfo::getFfmpegOutput()
{
    return ffmpegOutput;
}

QString FFMediaInfo::getFileName()
{
    return fileName;
}
