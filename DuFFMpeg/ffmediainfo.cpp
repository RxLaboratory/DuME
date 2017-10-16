#include "ffmediainfo.h"

#include <QtDebug>

FFMediaInfo::FFMediaInfo(QString ffmpegOutput, QObject *parent) : QObject(parent)
{
    updateInfo(ffmpegOutput);
}

void FFMediaInfo::updateInfo(QString ffmpegOutput)
{
    _container = QStringList();
    _fileName = "";
    _duration = 0.0;
    _videoWidth = 0;
    _videoHeight = 0;
    _videoFramerate = 0.0;
    _audioSamplingRate = 0;
    _size = 0;
    _videoCodec = nullptr;
    _audioCodec = nullptr;
    _audioBitrate = 0;
    _videoBitrate = 0;
    _imageSequence = false;
    _video = false;
    _audio = false;
    _ffmpegOptions.clear();
    _ffmpegOutput = ffmpegOutput;

    QStringList infos = ffmpegOutput.split("\n");


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
            _container = match.captured(1).split(",");
            _fileName = match.captured(2);
            qDebug() << info;
        }

        if (!input) continue;

        //test duration
        match = reDuration.match(info);
        if (match.hasMatch())
        {
            qDebug() << info;
            //set duration
            double h = match.captured(1).toDouble();
            double m = match.captured(2).toDouble();
            double s = match.captured(3).toDouble();
            _duration = h*60*60+m*60+s;
            qDebug() << "Duration" << _duration;
        }

        //test video stream
        match = reVideoStream.match(info);
        if (match.hasMatch())
        {
            qDebug() << info;
            //set size
            _videoWidth = match.captured(1).toInt();
            _videoHeight = match.captured(2).toInt();
            _videoFramerate = match.captured(3).toDouble();
            _video = true;
            continue;
        }

        //test audio stream
        match = reAudioStream.match(info);
        if (match.hasMatch())
        {
            qDebug() << info;
            //set sampling rate
            _audioSamplingRate = match.captured(1).toInt();
            _audio = true;
            continue;
        }
    }
}

void FFMediaInfo::setContainer(QStringList container)
{
    _container = container;
}

void FFMediaInfo::setVideoWidth(int width)
{
    _videoWidth = width;
}

void FFMediaInfo::setVideoHeight(int height)
{
    _videoHeight = height;
}

void FFMediaInfo::setVideoFramerate(double fps)
{
    _videoFramerate = fps;
}

void FFMediaInfo::setAudioSamplingRate(int sampling)
{
    _audioSamplingRate = sampling;
}

void FFMediaInfo::setDuration(double duration)
{
    _duration = duration;
}

void FFMediaInfo::setFileName(QString fileName)
{
    _fileName = fileName;
}

void FFMediaInfo::setVideoCodec(FFCodec *codec)
{
    _videoCodec = codec;
}

void FFMediaInfo::setAudioCodec(FFCodec *codec)
{
    _audioCodec = codec;
}

void FFMediaInfo::setVideoBitrate(double bitrate, BitrateUnit unit)
{
    if (unit == Kbps) bitrate = bitrate*1024;
    else if (unit == Mbps) bitrate = bitrate*1024*1024;
    _videoBitrate = bitrate;
}

void FFMediaInfo::setAudioBitrate(double bitrate, BitrateUnit unit)
{
    if (unit == Kbps) bitrate = bitrate*1024;
    else if (unit == Mbps) bitrate = bitrate*1024*1024;
    _audioBitrate = bitrate;
}

void FFMediaInfo::setSize(double size, FFMediaInfo::SizeUnit unit)
{

    if (unit == KB) size = size*1024;
    if (unit == MB) size = size*1024*1024;
    _size = size;
}

void FFMediaInfo::setFFmpegOptions(QStringList options)
{
    _ffmpegOptions = options;
}

void FFMediaInfo::setVideo(bool video)
{
    _video = video;
}

void FFMediaInfo::setAudio(bool audio)
{
    _audio = audio;
}

void FFMediaInfo::addFFmpegOption(QString option)
{
    _ffmpegOptions << option;
}

void FFMediaInfo::removeFFmpegOpstion(QString option)
{
    _ffmpegOptions.removeAll(option);
}

void FFMediaInfo::clearFFmpegOptions()
{
    _ffmpegOptions.clear();
}

QStringList FFMediaInfo::getContainer()
{
    return _container;
}

int FFMediaInfo::getVideoWidth()
{
    return _videoWidth;
}

int FFMediaInfo::getVideoHeight()
{
    return _videoHeight;
}

double FFMediaInfo::getVideoFramerate()
{
    return _videoFramerate;
}

int FFMediaInfo::getAudioSamplingRate()
{
    return _audioSamplingRate;
}

double FFMediaInfo::getDuration()
{
    return _duration;
}

QString FFMediaInfo::getFfmpegOutput()
{
    return _ffmpegOutput;
}

QString FFMediaInfo::getFileName()
{
    return _fileName;
}

FFCodec *FFMediaInfo::getVideoCodec()
{
    return _videoCodec;
}

FFCodec *FFMediaInfo::getAudioCodec()
{
    return _audioCodec;
}

double FFMediaInfo::getAudioBitrate(BitrateUnit unit)
{
    double bitrate = _audioBitrate;
    if (unit == Kbps) bitrate = bitrate/1024;
    if (unit == Mbps) bitrate = bitrate/1024/1024;
    return bitrate;
}

double FFMediaInfo::getVideoBitrate(BitrateUnit unit)
{
    double bitrate = _videoBitrate;
    if (unit == Kbps) bitrate = bitrate/1024;
    if (unit == Mbps) bitrate = bitrate/1024/1024;
    return bitrate;
}

double FFMediaInfo::getSize(FFMediaInfo::SizeUnit unit)
{
    double s = _size;
    if (unit == KB) s = s/1024;
    if (unit == MB) s = s/1024/1024;
    return s;
}

QStringList FFMediaInfo::getFFmpegOptions()
{
    return _ffmpegOptions;
}

bool FFMediaInfo::hasVideo()
{
    return _video;
}

bool FFMediaInfo::hasAudio()
{
    return _audio;
}

bool FFMediaInfo::isImageSequence()
{
    return _imageSequence;
}
