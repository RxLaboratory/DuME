#include "mediainfo.h"

MediaInfo::MediaInfo( FFmpeg *ffmpeg, QObject *parent ) : QObject(parent)
{
    _ffmpeg = ffmpeg;
    reInit();
}

MediaInfo::MediaInfo( FFmpeg *ffmpeg, QFileInfo mediaFile, QObject *parent ) : QObject(parent)
{
    _ffmpeg = ffmpeg;
    if ( mediaFile.suffix() == "dffp" ) loadPreset( mediaFile );
    else updateInfo( mediaFile );
}

void MediaInfo::reInit(bool removeFileName)
{
    // GENERAL
    if (removeFileName) _fileName = "";
    _extensions.clear();
    _muxer = nullptr;
    _duration = 0.0;
    _size = 0;
    _bitrate = 0;
    _video = false;
    _audio = false;
    _imageSequence = false;
    _videoCodec = nullptr;
    _videoWidth = 0;
    _videoHeight = 0;
    _videoFramerate = 24.0;
    _videoBitrate = 0;
    _pixAspect = 1;
    _videoAspect = 1.7777778;
    _pixFormat = nullptr;
    _audioCodec = nullptr;
    _audioSamplingRate = 0;
    _audioBitrate = 0;
    _audioChannels = "";
    _frames.clear();
    _videoQuality = -1;
    _videoProfile = -1;
    _loop = -1;
    _startNumber = 0;
    _premultipliedAlpha = true;
    _trc = "";
    // GENERAL Encoding/decoding parameters
    _cacheDir = nullptr;
    // FFMPEG Encoding/decoding
    _ffmpegOptions.clear();
    _ffmpegSequenceName = "";
    // AFTER EFFECTS Encoding/decoding
    _isAep = false;
    _aepCompName = "";
    _aepNumThreads = 1;
    _aepRqindex = 1;
    _aeUseRQueue = false;
}

void MediaInfo::updateInfo(QFileInfo mediaFile)
{
    reInit();

    QString extension = mediaFile.suffix();

    if (extension == "aep" || extension == "aet" || extension == "aepx")
    {
        setAep(true);
        _extensions << "aep" << "aet" << "aepx";
    }

    _fileName = QDir::toNativeSeparators( mediaFile.absoluteFilePath() ) ;
    _size = mediaFile.size();

    _muxer = _ffmpeg->muxer(extension);


    QString ffmpegOutput = _ffmpeg->analyseMedia( mediaFile.absoluteFilePath() );

    QStringList infos = ffmpegOutput.split("\n");

    //regexes to get infos
    QRegularExpression reInput("Input #\\d+, ([\\w+,]+) from '(.+)':");
    QRegularExpression reVideoStream("\\s*Stream #.+Video:\\s*([^,]*\\([^)]*\\)|[^,]*),\\s*([^,]*\\([^)]*\\)|[^,]*), (\\d+)x(\\d+) \\[SAR (\\d+):(\\d+) DAR (\\d+):(\\d+)][^,]*(?:,\\s*(\\d+)[^,]*,\\s+(\\d+) fps)?");
    QRegularExpression reAudioStream("\\s*Stream #.+Audio:\\s*([^,]*\\([^)]*\\)|[^,]*),\\s*(\\d*)\\s*Hz, ([^,]+),(?:.*,\\s*(?:(\\d+)\\s*kb\\/s)?)?");
    QRegularExpression reDuration("Duration: (?:(\\d\\d):(\\d\\d):(\\d\\d.\\d\\d),\\sstart:\\s\\d+.\\d+, bitrate:\\s(\\d+))?(?:(N\\/A), )?");

    bool input = false;
    foreach(QString info,infos)
    {
        //test input
        QRegularExpressionMatch match = reInput.match(info);
        if (match.hasMatch())
        {
            input = true;
            _extensions = match.captured(1).split(",");
            _fileName = QDir::toNativeSeparators( match.captured(2) );
        }

        if (!input) continue;

        //test duration
        match = reDuration.match(info);
        if (match.hasMatch())
        {
            if (match.captured(5) != "N/A")
            {
                //set duration
                int durationH = match.captured(1).toInt();
                int durationM = match.captured(2).toInt();
                double durationS = match.captured(3).toDouble();
                _duration = durationH*60*60+durationM*60+durationS;
                _bitrate = match.captured(4).toInt()*1000;
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
            QString codec = match.captured(1).left( match.captured(1).indexOf("(") );

            _videoCodec = _ffmpeg->videoEncoder( codec );
            if ( _videoCodec == nullptr ) _videoCodec = new FFCodec( codec );

            QString pixFormat = match.captured(2).left( match.captured(2).indexOf("(") );
            _pixFormat = _ffmpeg->pixFormat( pixFormat );
            if ( _pixFormat == nullptr ) _pixFormat = new FFPixFormat( pixFormat );

            _videoWidth = match.captured(3).toInt();
            _videoHeight = match.captured(4).toInt();
            _pixAspect = match.captured(5).toFloat() / match.captured(6).toFloat();
            _videoAspect = match.captured(7).toFloat() / match.captured(8).toFloat();
            _videoBitrate = match.captured(9).toInt()*1024;
            _videoFramerate = match.captured(10).toDouble();
            if ( int( _videoFramerate ) == 0 ) _videoFramerate = 24;
            _video = true;
            continue;
        }

        //test audio stream
        match = reAudioStream.match(info);
        if (match.hasMatch())
        {
            QString codec = match.captured(1).left( match.captured(1).indexOf("(") );

            _audioCodec = _ffmpeg->audioEncoder( codec );
            if ( _audioCodec == nullptr ) _audioCodec = new FFCodec( codec );

            _audioSamplingRate = match.captured(2).toInt();

            _audioChannels = match.captured(3);

            _audioBitrate = match.captured(4).toInt()*1024;

            _audio = true;
            continue;
        }
    }

    if (_imageSequence) loadSequence();
}

void MediaInfo::loadPreset(QFileInfo presetFile)
{
    QFile jsonFile( presetFile.absoluteFilePath() );

    QString json = "";

    if (jsonFile.open(QIODevice::ReadOnly))
    {
        json = jsonFile.readAll();
        jsonFile.close();
    }
    else return;

    qDebug() << "Loading preset:";
    qDebug() << json;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());

    //validate file
    if (!jsonDoc.isObject())
    {
        return;
    }

    QJsonObject mainObj = jsonDoc.object();
    if (mainObj.value("dume") == QJsonValue::Undefined)
    {
        return;
    }

    reInit(false);

    //load and create mediaInfo
    QJsonObject mediaObj = mainObj.value("dume").toObject();
    QString version = mediaObj.value("version").toString();
    //TODO Check version

    //muxer
    QJsonObject muxerObj = mediaObj.value("muxer").toObject();
    QString muxerName = muxerObj.value("name").toString();
    _muxer = _ffmpeg->muxer( muxerName );
    _loop = mediaObj.value("loop").toInt();

    //video
    if (mediaObj.value("hasVideo").toBool())
    {
        _video = true;
        QJsonObject videoObj = mediaObj.value("video").toObject();
        QString codecName = videoObj.value("codecName").toString();
        if (codecName != "default")
        {
           _videoCodec = _ffmpeg->videoEncoder( codecName );
        }
        _videoWidth = videoObj.value("width").toInt();
        _videoHeight = videoObj.value("height").toInt();
        _videoFramerate = videoObj.value("framerate").toDouble();
        _videoBitrate = videoObj.value("bitrate").toInt();
        _videoProfile = videoObj.value("profile").toInt();
        _videoQuality = videoObj.value("quality").toInt();
        _startNumber = videoObj.value("startNumber").toInt();
        if (!videoObj.value("premultipliedAlpha").isUndefined()) _premultipliedAlpha = videoObj.value("premultipliedAlpha").toBool();
        else _premultipliedAlpha = true;
        _pixFormat = _ffmpeg->pixFormat( videoObj.value("pixelFormat").toString() );
        if ( _pixFormat == nullptr ) qDebug() << "Aucun";
        else qDebug() << _pixFormat->prettyName();
    }

    //audio
    if (mediaObj.value("hasAudio").toBool())
    {
        _audio = true;
        QJsonObject audioObj = mediaObj.value("audio").toObject();
        QString codecName = audioObj.value("codecName").toString();
        if (codecName != "default")
        {
           _audioCodec = _ffmpeg->audioEncoder( codecName ) ;
        }
        _audioSamplingRate = audioObj.value("sampling").toInt();
        _audioBitrate = audioObj.value("bitrate").toInt();
    }

    //options
    QJsonArray options = mediaObj.value("options").toArray();
    foreach(QJsonValue option,options)
    {
        QJsonObject optionObj = option.toObject();
        QStringList opt(optionObj.value("name").toString());
        opt << optionObj.value("value").toString();
        addFFmpegOption(opt);
    }
    qDebug() << "Preset loaded.";
}

void MediaInfo::setVideoWidth(int width)
{
    _videoWidth = width;
}

void MediaInfo::setVideoHeight(int height)
{
    _videoHeight = height;
}

void MediaInfo::setVideoFramerate(double fps)
{
    _videoFramerate = fps;
}

void MediaInfo::setAudioSamplingRate(int sampling)
{
    _audioSamplingRate = sampling;
}

void MediaInfo::setDuration(double duration)
{
    _duration = duration;
}

void MediaInfo::setFileName(QString fileName)
{
    _fileName = fileName;
}

void MediaInfo::setVideoCodec(FFCodec *codec)
{
    _videoCodec = codec;
}

void MediaInfo::setAudioCodec(FFCodec *codec)
{
    _audioCodec = codec;
}

void MediaInfo::setVideoBitrate(double bitrate, MediaUtils::BitrateUnit unit)
{
    if (unit == MediaUtils::Kbps) bitrate = bitrate*1024;
    else if (unit == MediaUtils::Mbps) bitrate = bitrate*1024*1024;
    _videoBitrate = int( bitrate );
}

void MediaInfo::setAudioBitrate(double bitrate, MediaUtils::BitrateUnit unit)
{
    if (unit == MediaUtils::Kbps) bitrate = bitrate*1024;
    else if (unit == MediaUtils::Mbps) bitrate = bitrate*1024*1024;
    _audioBitrate = int( bitrate );
}

void MediaInfo::setSize(double size, MediaUtils::SizeUnit unit)
{

    if (unit == MediaUtils::KB) size = size*1024;
    else if (unit == MediaUtils::MB) size = size*1024*1024;
    else if (unit == MediaUtils::GB) size = size*1024*1024*1024;
    _size = qint64( size );
}

void MediaInfo::setFFmpegOptions(QList<QStringList> options)
{
    _ffmpegOptions = options;
}

void MediaInfo::setVideo(bool video)
{
    _video = video;
}

void MediaInfo::setAudio(bool audio)
{
    _audio = audio;
}

void MediaInfo::setMuxer(FFMuxer *muxer)
{
    _muxer = muxer;
}

void MediaInfo::addFFmpegOption(QStringList option)
{
    _ffmpegOptions << option;
}

void MediaInfo::removeFFmpegOptions(QString optionName)
{
    for(int i = _ffmpegOptions.count()-1 ; i >= 0 ; i--)
    {
        if (_ffmpegOptions[i][0] == optionName)
        {
            _ffmpegOptions.removeAt(i);
        }
    }
}

void MediaInfo::clearFFmpegOptions()
{
    _ffmpegOptions.clear();
}

int MediaInfo::videoWidth()
{
    return _videoWidth;
}

int MediaInfo::videoHeight()
{
    return _videoHeight;
}

double MediaInfo::videoFramerate()
{
    return _videoFramerate;
}

int MediaInfo::audioSamplingRate()
{
    return _audioSamplingRate;
}

double MediaInfo::duration()
{
    return _duration;
}

QString MediaInfo::fileName()
{
    return _fileName;
}

FFCodec *MediaInfo::videoCodec()
{
    return _videoCodec;
}

FFCodec *MediaInfo::audioCodec()
{
    return _audioCodec;
}

qint64 MediaInfo::audioBitrate()
{
    return _audioBitrate;
}

qint64 MediaInfo::videoBitrate()
{
    return _videoBitrate;
}

qint64 MediaInfo::size()
{
    return _size;
}

QList<QStringList> MediaInfo::ffmpegOptions()
{
    return _ffmpegOptions;
}

bool MediaInfo::hasVideo()
{
    return _video;
}

bool MediaInfo::hasAudio()
{
    return _audio;
}

bool MediaInfo::isImageSequence()
{
    return _imageSequence;
}

QStringList MediaInfo::extensions() const
{
    if (_muxer == nullptr) return _extensions;
    if (_muxer->extensions().count() == 0) return _extensions;
    return _muxer->extensions();
}

FFMuxer *MediaInfo::muxer() const
{
    return _muxer;
}

QString MediaInfo::exportPreset()
{
    QJsonObject mediaObj;
    mediaObj.insert("version", APPVERSION);

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
    mainObj.insert("dume",mediaObj);
    QJsonDocument jsonDoc(mainObj);
    return jsonDoc.toJson();
}

void MediaInfo::exportPreset(QString jsonPath)
{
    QFile jsonFile(jsonPath);
    if (jsonFile.open(QIODevice::WriteOnly))
    {
        jsonFile.write(exportPreset().toUtf8());
        jsonFile.close();
    }
}

QString MediaInfo::audioChannels() const
{
    return _audioChannels;
}

float MediaInfo::videoAspect() const
{
    return _videoAspect;
}

float MediaInfo::pixAspect() const
{
    return _pixAspect;
}

qint64 MediaInfo::bitrate() const
{
    return _bitrate;
}

QString MediaInfo::ffmpegSequenceName() const
{
    return _ffmpegSequenceName;
}

QTemporaryDir *MediaInfo::cacheDir() const
{
    return _cacheDir;
}

void MediaInfo::setCacheDir(QTemporaryDir *aepTempDir)
{
    _cacheDir = aepTempDir;
}

bool MediaInfo::aeUseRQueue() const
{
    return _aeUseRQueue;
}

void MediaInfo::setAeUseRQueue(bool aeUseRQueue)
{
    _aeUseRQueue = aeUseRQueue;
}

int MediaInfo::aepRqindex() const
{
    return _aepRqindex;
}

void MediaInfo::setAepRqindex(int aepRqindex)
{
    _aepRqindex = aepRqindex;
}

int MediaInfo::aepNumThreads() const
{
    return _aepNumThreads;
}

void MediaInfo::setAepNumThreads(int aepNumThreads)
{
    _aepNumThreads = aepNumThreads;
}

QString MediaInfo::aepCompName() const
{
    return _aepCompName;
}

void MediaInfo::setAepCompName(const QString &aepCompName)
{
    _aepCompName = aepCompName;
}

bool MediaInfo::isAep() const
{
    return _isAep;
}

void MediaInfo::setAep(bool isAep)
{
    _isAep = isAep;
}

QString MediaInfo::trc() const
{
    return _trc;
}

void MediaInfo::setTrc(const QString &trc)
{
    _trc = trc;
}

bool MediaInfo::premultipliedAlpha() const
{
    return _premultipliedAlpha;
}

void MediaInfo::setPremultipliedAlpha(bool premultipliedAlpha)
{
    _premultipliedAlpha = premultipliedAlpha;
}

FFPixFormat *MediaInfo::pixFormat()
{
    return _pixFormat;
}

void MediaInfo::setPixFormat(FFPixFormat *pixFormat)
{
    _pixFormat = pixFormat;
}

QStringList MediaInfo::frames() const
{
    return _frames;
}

void MediaInfo::setFrames(const QStringList &frames)
{
    _frames = frames;
}

void MediaInfo::loadSequence()
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
            _size = 0;
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
            _bitrate = ( _size * 8 ) / ( _frames.count()/24 );
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

    //generates the sequence name used by ffmpeg
    //detects all {###}
    QRegularExpression regExDigits("{(#+)}");
    QRegularExpressionMatchIterator regExDigitsMatch = regExDigits.globalMatch(_fileName);
    _ffmpegSequenceName = _fileName;
    while (regExDigitsMatch.hasNext())
    {
         QRegularExpressionMatch match = regExDigitsMatch.next();
         //count the ###
         QString digits = match.captured(1);
         int numDigits = digits.count();
         //replace
         _ffmpegSequenceName.replace(match.capturedStart(),match.capturedLength(),"%" + QString::number(numDigits) + "d");
    }
}

int MediaInfo::startNumber() const
{
    return _startNumber;
}

void MediaInfo::setStartNumber(int startNumber)
{
    _startNumber = startNumber;
}

int MediaInfo::videoQuality() const
{
    return _videoQuality;
}

void MediaInfo::setVideoQuality(int quality)
{
    _videoQuality = quality;
}

int MediaInfo::videoProfile() const
{
    return _videoProfile;
}

void MediaInfo::setVideoProfile(int profile)
{
    _videoProfile = profile;
}

int MediaInfo::loop() const
{
    return _loop;
}

void MediaInfo::setLoop(int loop)
{
    _loop = loop;
}


