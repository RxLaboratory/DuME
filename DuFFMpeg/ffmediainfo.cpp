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
    size = 0;
    audioBitrate = 0;
    videoBitrate = 0;
    videoCodec = FFCodec();
    audioCodec = FFCodec("","",false);
    imageSequence = false;
    video = false;
    audio = false;


    QStringList infos = ffmpeg.split("\n");


    //regexes to get infos
    QRegularExpression reInput("Input #\\d+, ([\\w+,]+) from '(.+)':");
    QRegularExpression reVideoStream("Stream #.+Video: .+, (\\d+)x(\\d+).+, (\\d{1,2}.?\\d{0,2}) fps");
    QRegularExpression reAudioStream("Stream #.+Audio: .+, (\\d{4,6}) Hz");
    QRegularExpression reDuration("  Duration: (\\d\\d):(\\d\\d):(\\d\\d.\\d\\d), ");


    bool input = false;
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
            video = true;
            continue;
        }

        //test audio stream
        match = reAudioStream.match(info);
        if (match.hasMatch())
        {
            qDebug() << info;
            //set sampling rate
            audioSamplingRate = match.captured(1).toInt();
            audio = true;
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

void FFMediaInfo::setVideoCodec(FFCodec codec)
{
    videoCodec = codec;
}

void FFMediaInfo::setAudioCodec(FFCodec codec)
{
    audioCodec = codec;
}

void FFMediaInfo::setVideoBitrate(double bitrate, BitrateUnit unit)
{
    if (unit == Kbps) bitrate = bitrate*1024;
    else if (unit == Mbps) bitrate = bitrate*1024*1024;
    videoBitrate = bitrate;
}

void FFMediaInfo::setAudioBitrate(double bitrate, BitrateUnit unit)
{
    if (unit == Kbps) bitrate = bitrate*1024;
    else if (unit == Mbps) bitrate = bitrate*1024*1024;
    audioBitrate = bitrate;
}

void FFMediaInfo::setSize(double s, FFMediaInfo::SizeUnit unit)
{

    if (unit == KB) s = s*1024;
    if (unit == MB) s = s*1024*1024;
    size = s;
}

void FFMediaInfo::setFFmpegOptions(QStringList options)
{
    ffmpegOptions = options;
}

void FFMediaInfo::setVideo(bool v)
{
    video = v;
}

void FFMediaInfo::setAudio(bool a)
{
    audio = a;
}

void FFMediaInfo::addFFmpegOption(QString option)
{
    ffmpegOptions << option;
}

void FFMediaInfo::removeFFmpegOpstion(QString option)
{
    ffmpegOptions.removeAll(option);
}

void FFMediaInfo::clearFFmpegOptions()
{
    ffmpegOptions.clear();
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

FFCodec FFMediaInfo::getVideoCodec()
{
    return videoCodec;
}

FFCodec FFMediaInfo::getAudioCodec()
{
    return audioCodec;
}

double FFMediaInfo::getAudioBitrate(BitrateUnit unit)
{
    double bitrate = audioBitrate;
    if (unit == Kbps) bitrate = bitrate/1024;
    if (unit == Mbps) bitrate = bitrate/1024/1024;
    return bitrate;
}

double FFMediaInfo::getVideoBitrate(BitrateUnit unit)
{
    double bitrate = videoBitrate;
    if (unit == Kbps) bitrate = bitrate/1024;
    if (unit == Mbps) bitrate = bitrate/1024/1024;
    return bitrate;
}

double FFMediaInfo::getSize(FFMediaInfo::SizeUnit unit)
{
    double s = size;
    if (unit == KB) s = s/1024;
    if (unit == MB) s = s/1024/1024;
    return s;
}

QStringList FFMediaInfo::getFFmpegOptions()
{
    return ffmpegOptions;
}

bool FFMediaInfo::hasVideo()
{
    return video;
}

bool FFMediaInfo::hasAudio()
{
    return audio;
}

bool FFMediaInfo::isImageSequence()
{
    return imageSequence;
}
