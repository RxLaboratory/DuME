#include "ffmediainfo.h"

#include <QtDebug>

FFMediaInfo::FFMediaInfo(QString ffmpegOutput, QObject *parent) : FFObject(parent)
{

    _cacheDir = nullptr;
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
    _pixFormat = nullptr;
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
    _startNumber = 0;
    _premultipliedAlpha = true;
    _trc = "";
    _isAep = false;
    _aepCompName = "";
    _aepNumThreads = 1;
    _aepRqindex = 1;
    _aeUseRQueue = false;

    QStringList infos = ffmpegOutput.split("\n");

    //regexes to get infos
    QRegularExpression reInput("Input #\\d+, ([\\w+,]+) from '(.+)':");
    QRegularExpression reVideoStream("Stream #.+Video: (.+)");
    QRegularExpression reSequenceStream("Stream #.+Video: .+, (\\d+)x(\\d+)");
    QRegularExpression reAudioStream("Stream #.+Audio: .+, (\\d{4,6}) Hz");
    QRegularExpression reDuration("Duration: (?:(\\d\\d):(\\d\\d):(\\d\\d.\\d\\d), )?(?:(N\\/A), )?");


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
            if (match.captured(4) != "N/A")
            {
                //set duration
                _durationH = match.captured(1).toInt();
                _durationM = match.captured(2).toInt();
                _durationS = match.captured(3).toDouble();
                _duration = _durationH*60*60+_durationM*60+_durationS;
            }
            else
            {
                _imageSequence = true;
                _videoFramerate = 24;
            }
        }

        //test video stream
        match = reVideoStream.match(info);
        if (match.hasMatch())
        {
            QString details = match.captured(1);
            QStringList detailsList = details.split(", ");
            qDebug() << details;
            QRegularExpression reVideoSize(", (\\d+)x(\\d+).*, ");
            QRegularExpression reVideoFrameRate(", (\\d{1,3}(?:\\.\\d{0,3})?) fps");
            QString codec = detailsList[0].split(" ")[0];
            _videoCodec = new FFCodec(codec);
            QString pf = detailsList[1];
            _pixFormat = new FFPixFormat(pf);
            QRegularExpressionMatch videoSizeMatch = reVideoSize.match(details);
            _videoWidth = videoSizeMatch.captured(1).toInt();
            _videoHeight = videoSizeMatch.captured(2).toInt();
            QRegularExpressionMatch videoFrameRateMatch = reVideoFrameRate.match(details);
            _videoFramerate = videoFrameRateMatch.captured(1).toDouble();
            if (_videoFramerate == 0) _videoFramerate = 24;
            _video = true;
            continue;
        }

        //test image sequence
        match = reSequenceStream.match(info);
        if (match.hasMatch())
        {
            qDebug() << info;
            //set size
            _videoWidth = match.captured(1).toInt();
            _videoHeight = match.captured(2).toInt();
            _videoFramerate = 24;
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

    if (_imageSequence) loadSequence();
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

void FFMediaInfo::removeFFmpegOptions(QString optionName)
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
        //start number
        videoObj.insert("startNumber",_startNumber);
        //pixel format
        videoObj.insert("pixelFormat",_pixFormat->name());
        //unmult
        videoObj.insert("premultipliedAlpha",_premultipliedAlpha);

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

void FFMediaInfo::exportToJson(QString jsonPath)
{
    QFile jsonFile(jsonPath);
    if (jsonFile.open(QIODevice::WriteOnly))
    {
        jsonFile.write(exportToJson().toUtf8());
        jsonFile.close();
    }
}

int FFMediaInfo::durationH() const
{
    return _durationH;
}

void FFMediaInfo::setDurationH(int durationH)
{
    _durationH = durationH;
}

int FFMediaInfo::durationM() const
{
    return _durationM;
}

void FFMediaInfo::setDurationM(int durationM)
{
    _durationM = durationM;
}

double FFMediaInfo::durationS() const
{
    return _durationS;
}

void FFMediaInfo::setDurationS(double durationS)
{
    _durationS = durationS;
}

int FFMediaInfo::durationF() const
{
    return _durationF;
}

void FFMediaInfo::setDurationF(int durationI)
{
    _durationF = durationI;
}

QTemporaryDir *FFMediaInfo::cacheDir() const
{
    return _cacheDir;
}

void FFMediaInfo::setCacheDir(QTemporaryDir *aepTempDir)
{
    _cacheDir = aepTempDir;
}

bool FFMediaInfo::aeUseRQueue() const
{
    return _aeUseRQueue;
}

void FFMediaInfo::setAeUseRQueue(bool aeUseRQueue)
{
    _aeUseRQueue = aeUseRQueue;
}

int FFMediaInfo::aepRqindex() const
{
    return _aepRqindex;
}

void FFMediaInfo::setAepRqindex(int aepRqindex)
{
    _aepRqindex = aepRqindex;
}

int FFMediaInfo::aepNumThreads() const
{
    return _aepNumThreads;
}

void FFMediaInfo::setAepNumThreads(int aepNumThreads)
{
    _aepNumThreads = aepNumThreads;
}

QString FFMediaInfo::aepCompName() const
{
    return _aepCompName;
}

void FFMediaInfo::setAepCompName(const QString &aepCompName)
{
    _aepCompName = aepCompName;
}

bool FFMediaInfo::isAep() const
{
    return _isAep;
}

void FFMediaInfo::setAep(bool isAep)
{
    _isAep = isAep;
}

QString FFMediaInfo::trc() const
{
    return _trc;
}

void FFMediaInfo::setTrc(const QString &trc)
{
    _trc = trc;
}

bool FFMediaInfo::premultipliedAlpha() const
{
    return _premultipliedAlpha;
}

void FFMediaInfo::setPremultipliedAlpha(bool premultipliedAlpha)
{
    _premultipliedAlpha = premultipliedAlpha;
}

FFPixFormat *FFMediaInfo::pixFormat()
{
    return _pixFormat;
}

void FFMediaInfo::setPixFormat(FFPixFormat *pixFormat)
{
    _pixFormat = pixFormat;
}

QStringList FFMediaInfo::frames() const
{
    return _frames;
}

void FFMediaInfo::setFrames(const QStringList &frames)
{
    _frames = frames;
}

void FFMediaInfo::loadSequence()
{
    _frames.clear();
    _startNumber = 0;

    if (_fileName == "") return;

    //base file info
    QFileInfo baseFileInfo(_fileName);
    QString extension = baseFileInfo.suffix();
    QString baseName = baseFileInfo.completeBaseName();
    QString dirPath = baseFileInfo.path();


    //find digits in the name
    QRegularExpression reDigits("(\\d+)");
    QRegularExpressionMatchIterator reDigitsMatch = reDigits.globalMatch(baseName);

    //if we didn't find any digit in the name, nothing to do
    if (!reDigitsMatch.hasNext()) return;

    //get all files of same type in folder
    QDir containingDir(dirPath);
    QFileInfoList files = containingDir.entryInfoList(QStringList("*." + extension),QDir::Files);

    //find where is the base file
    int baseIndex = 0;
    for (int i = 0 ; i < files.count() ; i++)
    {
        QFileInfo file = files[i];
        if (file.completeBaseName() == baseName)
        {
            baseIndex = i;
            break;
        }
    }

    //check for each block of digits the one where we have a next and previous file
    while(reDigitsMatch.hasNext())
    {
        QRegularExpressionMatch match = reDigitsMatch.next();
        QString digits = match.captured(0);
        int num = digits.toInt();
        bool ok = false;
        //test next file
        if (baseIndex < files.count()-1)
        {
            QString nextName = files[baseIndex+1].completeBaseName();
            //check if num+1 is at the same place
            int nextNum = num+1;
            QString nextDigits = QString::number(nextNum);
            while(nextDigits.count() < digits.count())
            {
                nextDigits = "0" + nextDigits;
            }
            if (nextName.indexOf(nextDigits) == match.capturedStart(0)) ok = true;
        }
        else
        {
            ok = true;
        }

        //test previous file
        if (ok)
        {
            ok = false;
            if (baseIndex > 0)
            {
                QString prevName = files[baseIndex-1].completeBaseName();
                //check if num-1 is at the same place
                int prevNum = num-1;
                QString prevDigits = QString::number(prevNum);
                while(prevDigits.count() < digits.count())
                {
                    prevDigits = "0" + prevDigits;
                }
                if (prevName.indexOf(prevDigits) == match.capturedStart(0)) ok = true;
            }
            else
            {
                ok = true;
            }
        }

        //if next and previous file are ok, we found the digits block
        if (ok)
        {
            //list all files in the sequence matching the pattern, and compute size
            QString left = baseName.left(match.capturedStart(0));
            QString right = baseName.right(baseName.count() - match.capturedEnd(0));
            QString pattern = left + "(\\d+)" + right;
            QRegularExpression re(pattern);
            _startNumber = 999999999;
            foreach(QFileInfo f,files)
            {
                QRegularExpressionMatch reMatch = re.match(f.completeBaseName());
                if (reMatch.hasMatch())
                {
                    //get start frame
                    int currentNumber = reMatch.captured(1).toInt();
                    if (currentNumber < _startNumber) _startNumber = currentNumber;
                    _frames << f.filePath();
                    _size += f.size();
                }
            }
            if (_startNumber == 999999999) _startNumber = 0;
            //update filename with ffmpeg convention
            QString digitsBlock = "";
            while(digitsBlock.count() < digits.count())
            {
                digitsBlock += "#";
            }
            _fileName = dirPath + "/" + left + "{" + digitsBlock + "}" + right + "." + extension;
        }
    }
}

int FFMediaInfo::startNumber() const
{
    return _startNumber;
}

void FFMediaInfo::setStartNumber(int startNumber)
{
    _startNumber = startNumber;
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


