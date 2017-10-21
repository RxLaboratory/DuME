#include "ffmediainfo.h"

#include <QtDebug>

FFMediaInfo::FFMediaInfo(QString ffmpegOutput, QObject *parent) : FFObject(parent)
{
    updateInfo(ffmpegOutput);
}

void FFMediaInfo::updateInfo(QString ffmpegOutput)
{
    _muxer = nullptr;
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
    _videoQuality = -1;
    _loop = -1;
    _videoProfile = -1;

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
            _extensions = match.captured(1).split(",");
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

void FFMediaInfo::setFFmpegOptions(QList<QStringList> options)
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

void FFMediaInfo::setMuxer(FFMuxer *muxer)
{
    _muxer = muxer;
}

void FFMediaInfo::addFFmpegOption(QStringList option)
{
    _ffmpegOptions << option;
}

void FFMediaInfo::removeFFmpegOpstion(QString optionName)
{
    for(int i = _ffmpegOptions.count()-1 ; i >= 0 ; i--)
    {
        if (_ffmpegOptions[i][0] == optionName)
        {
            _ffmpegOptions.removeAt(i);
        }
    }
}

void FFMediaInfo::clearFFmpegOptions()
{
    _ffmpegOptions.clear();
}

int FFMediaInfo::videoWidth()
{
    return _videoWidth;
}

int FFMediaInfo::videoHeight()
{
    return _videoHeight;
}

double FFMediaInfo::videoFramerate()
{
    return _videoFramerate;
}

int FFMediaInfo::audioSamplingRate()
{
    return _audioSamplingRate;
}

double FFMediaInfo::duration()
{
    return _duration;
}

QString FFMediaInfo::ffmpegOutput()
{
    return _ffmpegOutput;
}

QString FFMediaInfo::fileName()
{
    return _fileName;
}

FFCodec *FFMediaInfo::videoCodec()
{
    return _videoCodec;
}

FFCodec *FFMediaInfo::audioCodec()
{
    return _audioCodec;
}

double FFMediaInfo::audioBitrate(BitrateUnit unit)
{
    double bitrate = _audioBitrate;
    if (unit == Kbps) bitrate = bitrate/1024;
    if (unit == Mbps) bitrate = bitrate/1024/1024;
    return bitrate;
}

double FFMediaInfo::videoBitrate(BitrateUnit unit)
{
    double bitrate = _videoBitrate;
    if (unit == Kbps) bitrate = bitrate/1024;
    if (unit == Mbps) bitrate = bitrate/1024/1024;
    return bitrate;
}

double FFMediaInfo::size(FFMediaInfo::SizeUnit unit)
{
    double s = _size;
    if (unit == KB) s = s/1024;
    if (unit == MB) s = s/1024/1024;
    return s;
}

QList<QStringList> FFMediaInfo::ffmpegOptions()
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

QStringList FFMediaInfo::extensions() const
{
    if (_muxer == nullptr) return _extensions;
    if (_muxer->extensions().count() == 0) return _extensions;
    return _muxer->extensions();
}

FFMuxer *FFMediaInfo::muxer() const
{
    return _muxer;
}

QString FFMediaInfo::exportToJson()
{
    QJsonObject mediaObj;
    mediaObj.insert("version",DUFFMPEG_VERSION);

    //muxer
    QJsonObject muxerObj;
    QString muxerName = "";
    QString muxerPrettyName = "";
    bool muxerIsSequence = false;
    if (_muxer != nullptr)
    {
        muxerName = _muxer->name();
        muxerPrettyName = _muxer->prettyName();
        muxerIsSequence = _muxer->isSequence();
    }
    muxerObj.insert("name",muxerName);
    muxerObj.insert("prettyName",muxerPrettyName);
    muxerObj.insert("isSequence",muxerIsSequence);
    QJsonArray muxerExtensions = QJsonArray::fromStringList(extensions());
    muxerObj.insert("extensions",muxerExtensions);
    //insert
    mediaObj.insert("muxer",muxerObj);

    //has video / audio
    mediaObj.insert("hasVideo",hasVideo());
    mediaObj.insert("hasAudio",hasAudio());

    //video
    if (hasVideo())
    {
        //name
        QJsonObject videoObj;
        QString videoCodecName = "default";
        QString videoCodecPrettyName = "Default codec";
        if (_videoCodec != nullptr)
        {
            videoCodecName = _videoCodec->name();
            videoCodecPrettyName = _videoCodec->prettyName();
        }
        videoObj.insert("codecName",videoCodecName);
        videoObj.insert("codecPrettyName",videoCodecPrettyName);
        //resize
        videoObj.insert("width",_videoWidth);
        videoObj.insert("height",_videoHeight);
        //framerate
        videoObj.insert("framerate",_videoFramerate);
        //bitrate
        videoObj.insert("bitrate",_videoBitrate);
        //quality
        videoObj.insert("quality",_videoQuality);
        //loop
        videoObj.insert("loop",_loop);
        //profile
        videoObj.insert("profile",_videoProfile);

        mediaObj.insert("video",videoObj);
    }

    //audio
    if (hasAudio())
    {
        //name
        QJsonObject audioObj;
        QString audioCodecName = "default";
        QString audioCodecPrettyName = "Default codec";
        if (_audioCodec != nullptr)
        {
            audioCodecName = _audioCodec->name();
            audioCodecPrettyName = _audioCodec->prettyName();
        }
        audioObj.insert("codecName",audioCodecName);
        audioObj.insert("codecPrettyName",audioCodecPrettyName);
        //resample
        audioObj.insert("sampling",_audioSamplingRate);
        //bitrate
        audioObj.insert("bitrate",_audioBitrate);

        mediaObj.insert("audio",audioObj);
    }

    //options
    QJsonArray options;
    foreach (QStringList option, _ffmpegOptions) {
        QJsonObject optionObj;
        optionObj.insert("name",option[0]);
        QString optionValue = "";
        if (option.count() > 1)
        {
            optionValue = option[1];
        }
        optionObj.insert("value",optionValue);
        options.append(optionObj);
    }
    //insert
    mediaObj.insert("options",options);

    QJsonObject mainObj;
    mainObj.insert("duffmpeg",mediaObj);
    QJsonDocument jsonDoc(mainObj);
    return jsonDoc.toJson();
}

void FFMediaInfo::exportToJson(QFile jsonFile)
{
    if (jsonFile.open(QIODevice::WriteOnly))
    {
        jsonFile.write(exportToJson().toUtf8());
        jsonFile.close();
    }
}

int FFMediaInfo::videoQuality() const
{
    return _videoQuality;
}

void FFMediaInfo::setVideoQuality(int quality)
{
    _videoQuality = quality;
}

int FFMediaInfo::videoProfile() const
{
    return _videoProfile;
}

void FFMediaInfo::setVideoProfile(int profile)
{
    _videoProfile = profile;
}

int FFMediaInfo::loop() const
{
    return _loop;
}

void FFMediaInfo::setLoop(int loop)
{
    _loop = loop;
}


