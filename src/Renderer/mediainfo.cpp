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

void MediaInfo::reInit(bool removeFileName, bool silent)
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
    _videoFramerate = 0.0;
    _videoBitrate = 0;
    _pixAspect = 1;
    _videoAspect = 0.0;
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
    _colorSpace = "";
    _colorTRC = "";
    _colorPrimaries = "";
    _colorRange = "";
    _videoLanguage = "";
    _audioLanguage = "";
    // STREAMS
    qDeleteAll(_videoStreams);
    _videoStreams.clear();
    qDeleteAll(_audioStreams);
    _audioStreams.clear();
    // MAPS
    _maps.clear();
    // GENERAL Encoding/decoding parameters
    _cacheDir = nullptr;
    // FFMPEG Encoding/decoding
    _ffmpegOptions.clear();
    _ffmpegSequenceName = "";
    // AFTER EFFECTS Encoding/decoding
    _isAep = false;
    _aepCompName = "";
    _aepNumThreads = 1;
    _aepRqindex = -1;
    _aeUseRQueue = false;

    if(!silent) emit changed();
}

void MediaInfo::updateInfo(QFileInfo mediaFile, bool silent)
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
    QRegularExpression reInput = RegExUtils::getRegEx("ffmpeg input");
    QRegularExpression reVideoStream = RegExUtils::getRegEx("ffmpeg video stream");
    QRegularExpression reAudioStream = RegExUtils::getRegEx("ffmpeg audio stream");
    QRegularExpression reDuration = RegExUtils::getRegEx("ffmpeg duration");

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
            //add the stream
            VideoInfo *stream = new VideoInfo;
            stream->setId( match.captured(1).toInt() );
            stream->setLanguage( (match.captured(2)) );

            QString codec = match.captured(3).left( match.captured(3).indexOf("(") );
            FFCodec *c = _ffmpeg->videoEncoder( codec );
            if (c == nullptr ) c = new FFCodec(codec);
            stream->setCodec( c );

            QString pixFormat = match.captured(4).left( match.captured(4).indexOf("(") );
            FFPixFormat *pf = _ffmpeg->pixFormat( pixFormat );
            if ( pf == nullptr ) pf = new FFPixFormat( pixFormat );
            stream->setPixFormat( pf );

            stream->setWidth( match.captured(5).toInt() );
            stream->setHeight( match.captured(6).toInt() );
            if ( match.captured(8) != "" ) stream->setPixAspect( match.captured(7).toFloat() / match.captured(8).toFloat() );
            if ( match.captured(10).toFloat() != 0.0 ) stream->setAspect( match.captured(9).toFloat() / match.captured(10).toFloat() );
            else if ( stream->height() != 0) stream->setAspect( double( stream->height() ) / double( stream->height() ) );

            stream->setBitrate( match.captured(11).toInt()*1024 );
            stream->setFramerate( match.captured(12).toDouble() );

            if ( int( stream->framerate() ) == 0 ) stream->setFramerate( 24 );
            _video = true;

            _videoStreams << stream;
            continue;
        }

        //test audio stream
        match = reAudioStream.match(info);
        if (match.hasMatch())
        {
            //add the stream
            AudioInfo *stream = new AudioInfo;
            stream->setId( match.captured(1).toInt() );
            stream->setLanguage( match.captured(2) );

            QString codec = match.captured(3).left( match.captured(3).indexOf("(") );
            FFCodec *c = _ffmpeg->audioEncoder( codec );
            if ( c == nullptr ) c = new FFCodec( codec );
            stream->setCodec(c);

            stream->setSamplingRate( match.captured(4).toInt() );

            stream->setChannels( match.captured(5) );

            stream->setBitrate( match.captured(6).toInt()*1024 );

            _audioStreams << stream;

            _audio = true;
            continue;
        }
    }

    if (_imageSequence) loadSequence();

    if(!silent) emit changed();
}

void MediaInfo::updateInfo(MediaInfo *other, bool updateFilename, bool silent)
{
    // GENERAL
    if (updateFilename) _fileName = other->fileName();
    _extensions = other->extensions();
    _muxer = other->muxer();
    _duration = other->duration();
    _size = other->size();
    _bitrate = other->bitrate();
    _video = other->hasVideo();
    _audio = other->hasAudio();
    _imageSequence = other->isImageSequence();
    _videoCodec = other->videoCodec();
    _videoWidth = other->videoWidth();
    _videoHeight = other->videoHeight();
    _videoFramerate = other->videoFramerate();
    _videoBitrate = other->videoBitrate();
    _pixAspect = other->pixAspect();
    _videoAspect = other->videoAspect();
    _pixFormat = other->pixFormat();
    _audioCodec = other->audioCodec();
    _audioSamplingRate = other->audioSamplingRate();
    _audioBitrate = other->audioBitrate();
    _audioChannels = other->audioChannels();
    _frames = other->frames();
    _videoQuality = other->videoQuality();
    _videoProfile = other->videoProfile();
    _loop = other->loop();
    _startNumber = other->startNumber();
    _premultipliedAlpha = other->premultipliedAlpha();
    _colorSpace = other->colorSpace();
    _colorTRC = other->colorTRC();
    _colorPrimaries = other->colorPrimaries();
    _colorRange = other->colorRange();
    // GENERAL Encoding/decoding parameters
    _cacheDir = other->cacheDir();
    // FFMPEG Encoding/decoding
    _ffmpegOptions = other->ffmpegOptions();
    _ffmpegSequenceName = other->ffmpegSequenceName();
    // AFTER EFFECTS Encoding/decoding
    _isAep = other->isAep();
    _aepCompName = other->aepCompName();
    _aepNumThreads = other->aepNumThreads();
    _aepRqindex = other->aepRqindex();
    _aeUseRQueue = other->aeUseRQueue();

    if(!silent) emit changed();
}

QString MediaInfo::getDescription()
{
    //Text
    QString mediaInfoString = "";

    QFileInfo fileInfo(_fileName);

    if ( _isAep )
    {
        if (fileInfo.suffix() == "aep") mediaInfoString += "After Effects project.";
        if (fileInfo.suffix() == "aet") mediaInfoString += "After Effects template.";
        if (fileInfo.suffix() == "aepx") mediaInfoString += "After Effects XML project.";
    }

    if ( _muxer != nullptr )
    {
        mediaInfoString += "File format: " + _muxer->prettyName();
    }
    else
    {
        mediaInfoString += "Containers: " + _extensions.join(",");
    }


    if (_duration != 0.0)
    {
        QTime duration(0,0,0);
        duration = duration.addSecs( int( _duration ) );
        mediaInfoString += "\nDuration: " + duration.toString("hh:mm:ss.zzz");
    }
    else if (_imageSequence)
    {
        mediaInfoString += "\nDuration: " + QString::number(  _frames.count() ) + " frames";
        mediaInfoString += "\nStart Frame Number: " + QString::number( _startNumber );
    }

    qint64 size = _size;
    if (size != 0) mediaInfoString += "\nSize: " + MediaUtils::sizeString( size );

    if ( !_isAep )
    {
        qint64 bitrate = _bitrate;
        if (bitrate != 0) mediaInfoString += "\nGlobal bitrate: " + MediaUtils::bitrateString( bitrate );

        mediaInfoString += "\nContains video: ";
        if (_video) mediaInfoString += "yes";
        else mediaInfoString += "no";
        mediaInfoString += "\nContains audio: ";
        if (_audio) mediaInfoString += "yes";
        else mediaInfoString += "no";
        mediaInfoString += "\nImage sequence: ";
        if (_imageSequence) mediaInfoString += "yes";
        else mediaInfoString += "no";

        //TEMP to be replaced by streams
        if (_video && _videoStreams.count() == 0)
        {
            mediaInfoString += "\n\nVideo stream 1:";
            if (_videoLanguage != "") mediaInfoString += "\nVideo language: " + LanguageUtils::get(_videoLanguage);
            mediaInfoString += "\nVideo codec: ";
            FFCodec *vc = _videoCodec;
            if (vc == nullptr) vc = defaultVideoCodec();
            if (vc != nullptr)
            {
                mediaInfoString += vc->prettyName();
            }

            if (_videoWidth !=0 || _videoHeight != 0 ) mediaInfoString += "\nResolution: " + QString::number(_videoWidth) + "x" + QString::number(_videoHeight);
            if (_videoAspect != 0 ) mediaInfoString += "\nVideo Aspect: " + QString::number( int( _videoAspect*100+0.5 ) / 100.0) + ":1";
            if (_videoFramerate != 0 ) mediaInfoString += "\nFramerate: " + QString::number(_videoFramerate) + " fps";
            qint64 bitrate = _videoBitrate;
            if (bitrate != 0) mediaInfoString += "\nBitrate: " + MediaUtils::bitrateString(bitrate);
            mediaInfoString += "\nPixel Aspect: " + QString::number( int(_pixAspect*100+0.5)/ 100.0) + ":1";
            FFPixFormat *pf = _pixFormat;
            if ( pf == nullptr ) pf = defaultPixFormat();
            if (pf != nullptr)
            {
                mediaInfoString += "\nPixel Format: " + pf->prettyName();
                if (pf->hasAlpha()) mediaInfoString += "\nAlpha: yes";
                else mediaInfoString += "\nAlpha: no";
            }
        }

        if (_audio && _audioStreams.count() == 0)
        {
            mediaInfoString += "\n\nAudio stream 1:";
            if (_audioLanguage != "") mediaInfoString += "\nAudio language: " + LanguageUtils::get(_audioLanguage);
            mediaInfoString += "\nAudio codec: ";
            FFCodec *ac = _audioCodec;
            if (ac == nullptr) ac = defaultAudioCodec();
            if(ac != nullptr)
            {
                mediaInfoString += ac->prettyName();
            }
            if (_audioSamplingRate != 0) mediaInfoString += "\nSampling rate: " + QString::number(_audioSamplingRate) + " Hz";
            if (_audioChannels != "")
            {
                mediaInfoString += "\nChannels: " + _audioChannels;
            }
            int abitrate = int( _audioBitrate );
            if (abitrate != 0) mediaInfoString += "\nBitrate: " + MediaUtils::bitrateString(abitrate);
        }

        for ( int i = 0; i < _videoStreams.count(); i++)
        {
            VideoInfo *s = _videoStreams[i];

            mediaInfoString += "\n\nVideo stream #" + QString::number( s->id() ) + ":";

            if ( s->language() != "") mediaInfoString += "\nVideo language: " + LanguageUtils::get( s->language() );
            mediaInfoString += "\nVideo codec: ";
            FFCodec *vc = s->codec();
            if (vc == nullptr) vc = defaultVideoCodec();
            if (vc != nullptr)
            {
                mediaInfoString += vc->prettyName();
            }

            if (s->width() !=0 || s->height() != 0 ) mediaInfoString += "\nResolution: " + QString::number( s->width() ) + "x" + QString::number( s->height() );
            if (s->aspect() != 0 ) mediaInfoString += "\nVideo Aspect: " + QString::number( int( s->aspect()*100+0.5 ) / 100.0) + ":1";
            if (s->framerate() != 0 ) mediaInfoString += "\nFramerate: " + QString::number(s->framerate()) + " fps";
            qint64 bitrate = s->bitrate();
            if (bitrate != 0) mediaInfoString += "\nBitrate: " + MediaUtils::bitrateString(bitrate);
            mediaInfoString += "\nPixel Aspect: " + QString::number( int(s->pixAspect()*100+0.5)/ 100.0) + ":1";
            FFPixFormat *pf = s->pixFormat();
            if ( pf == nullptr ) pf = defaultPixFormat();
            if (pf != nullptr)
            {
                mediaInfoString += "\nPixel Format: " + pf->prettyName();
                if (pf->hasAlpha()) mediaInfoString += "\nAlpha: yes";
                else mediaInfoString += "\nAlpha: no";
            }
        }

        for ( int i = 0; i < _audioStreams.count(); i++)
        {
            AudioInfo *s = _audioStreams[i];

            mediaInfoString += "\n\nAudio stream #" + QString::number( s->id() ) + ":";

            if ( s->language() != "") mediaInfoString += "\nAudio language: " + LanguageUtils::get(s->language());
            mediaInfoString += "\nAudio codec: ";
            FFCodec *ac = s->codec();
            if (ac == nullptr) ac = defaultAudioCodec();
            if(ac != nullptr)
            {
                mediaInfoString += ac->prettyName();
            }
            if (s->samplingRate() != 0) mediaInfoString += "\nSampling rate: " + QString::number( s->samplingRate() ) + " Hz";
            if (s->channels() != "")
            {
                mediaInfoString += "\nChannels: " + s->channels();
            }
            int abitrate = int( s->bitrate() );
            if (abitrate != 0) mediaInfoString += "\nBitrate: " + MediaUtils::bitrateString(abitrate);
        }
    }
    return mediaInfoString;
}

void MediaInfo::loadPreset(QFileInfo presetFile, bool silent)
{
    QFile jsonFile( presetFile.absoluteFilePath() );

    QString json = "";

    if (jsonFile.open(QIODevice::ReadOnly))
    {
        json = jsonFile.readAll();
        jsonFile.close();
    }
    else return;

    qDebug() << "Loading preset: " + presetFile.completeBaseName();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());

    //validate file
    if (!jsonDoc.isObject())
    {
        return;
    }

    QJsonObject mainObj = jsonDoc.object();
    if (mainObj.value("dume") == QJsonValue::Undefined && mainObj.value("duffmpeg") == QJsonValue::Undefined)
    {
        return;
    }

    reInit(false, true);

    //load and create mediaInfo
    QJsonObject mediaObj = mainObj.value("dume").toObject();
    QString version = mediaObj.value("version").toString();
    //TODO Check version

    //muxer
    QJsonObject muxerObj = mediaObj.value("muxer").toObject();

    setMuxer( muxerObj.value("name").toString(), true);
    setLoop( mediaObj.value("loop").toInt(), true );

    //video
    if (mediaObj.value("hasVideo").toBool())
    {
        setVideo( true, true);
        QJsonObject videoObj = mediaObj.value("video").toObject();
        QString codecName = videoObj.value("codecName").toString();
        if (codecName != "default")
        {
           setVideoCodec( codecName, true );
        }
        setVideoWidth( videoObj.value("width").toInt(), true );
        setVideoHeight( videoObj.value("height").toInt(), true );
        setVideoFramerate( videoObj.value("framerate").toDouble(), true );
        setVideoBitrate( videoObj.value("bitrate").toInt(), true );
        setVideoProfile( videoObj.value("profile").toInt(), true );
        setVideoQuality( videoObj.value("quality").toInt(), true );
        setStartNumber( videoObj.value("startNumber").toInt(), true );
        if (!videoObj.value("premultipliedAlpha").isUndefined()) setPremultipliedAlpha( videoObj.value("premultipliedAlpha").toBool(), true );
        setPixFormat( videoObj.value("pixelFormat").toString(), true );
        if (!videoObj.value("colorSpace").isUndefined()) setColorSpace( videoObj.value("colorSpace").toString(), true);
        if (!videoObj.value("colorRange").isUndefined()) setColorRange( videoObj.value("colorRange").toString(), true);
        if (!videoObj.value("colorTRC").isUndefined()) setColorTRC( videoObj.value("colorTRC").toString(), true);
        if (!videoObj.value("colorPrimaries").isUndefined()) setColorPrimaries( videoObj.value("colorSpace").toString(), true);
    }

    //audio
    if (mediaObj.value("hasAudio").toBool())
    {
        setAudio( true, true );
        QJsonObject audioObj = mediaObj.value("audio").toObject();
        QString codecName = audioObj.value("codecName").toString();
        if (codecName != "default")
        {
           setAudioCodec( codecName, true ) ;
        }
        setAudioSamplingRate( audioObj.value("sampling").toInt(), true );
        setAudioBitrate( audioObj.value("bitrate").toInt(), true );
    }

    //options
    QJsonArray options = mediaObj.value("options").toArray();
    foreach(QJsonValue option,options)
    {
        QJsonObject optionObj = option.toObject();
        QStringList opt(optionObj.value("name").toString());
        opt << optionObj.value("value").toString();
        addFFmpegOption(opt, true );
    }

    if(!silent) emit changed();
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
    QJsonArray muxerExtensions = QJsonArray::fromStringList( _extensions );
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
        if ( _pixFormat != nullptr ) videoObj.insert("pixelFormat",_pixFormat->name());
        //unmult
        videoObj.insert("premultipliedAlpha",_premultipliedAlpha);
        //colors
        videoObj.insert("colorSpace", _colorSpace);
        videoObj.insert("colorRange", _colorRange);
        videoObj.insert("colorTRC", _colorTRC);
        videoObj.insert("colorPrimaries", _colorPrimaries);

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

    qDebug() << "Generated Preset: ";
    qDebug() << jsonDoc;

    return jsonDoc.toJson();
}

void MediaInfo::exportPreset(QString jsonPath)
{
    qDebug() << "Saving Preset: " + jsonPath;

    QFile jsonFile(jsonPath);
    if (jsonFile.open(QIODevice::WriteOnly))
    {
        jsonFile.write(exportPreset().toUtf8());
        jsonFile.close();
    }
}

// SETTERS

void MediaInfo::setVideoWidth(int width, bool silent )
{
    _videoWidth = width;
    if(!silent) emit changed();
}

void MediaInfo::setVideoHeight(int height, bool silent )
{
    _videoHeight = height;
    if(!silent) emit changed();
}

void MediaInfo::setVideoFramerate(double fps, bool silent )
{
    _videoFramerate = fps;
    if(!silent) emit changed();
}

void MediaInfo::setAudioSamplingRate(int sampling, bool silent )
{
    _audioSamplingRate = sampling;
    if(!silent) emit changed();
}

void MediaInfo::setDuration(double duration, bool silent )
{
    _duration = duration;
    if(!silent) emit changed();
}

void MediaInfo::setFileName(QString fileName, bool silent )
{
    _fileName = fileName;
    if (_muxer != nullptr)
    {
        if ( _muxer->isSequence() ) loadSequence();
    }
    if(!silent) emit changed();
}

void MediaInfo::setVideoCodec(FFCodec *codec, bool silent )
{
    _videoCodec = codec;
    if(!silent) emit changed();
}

void MediaInfo::setVideoCodec(QString codecName, bool silent )
{
    setVideoCodec( _ffmpeg->videoEncoder( codecName ), silent );
}

void MediaInfo::setAudioCodec(FFCodec *codec, bool silent )
{
    _audioCodec = codec;
    if(!silent) emit changed();
}

void MediaInfo::setAudioCodec(QString codecName, bool silent )
{
    setAudioCodec( _ffmpeg->audioEncoder( codecName ), silent );
}

void MediaInfo::setVideoBitrate(qint64 bitrate, bool silent )
{
    _videoBitrate = bitrate;
    if(!silent) emit changed();
}

void MediaInfo::setAudioBitrate(qint64 bitrate, bool silent )
{
    _audioBitrate = bitrate;
    if(!silent) emit changed();
}

void MediaInfo::setSize(double size, MediaUtils::SizeUnit unit, bool silent )
{

    if (unit == MediaUtils::kB) size = size*1024;
    else if (unit == MediaUtils::MB) size = size*1024*1024;
    else if (unit == MediaUtils::GB) size = size*1024*1024*1024;
    _size = qint64( size );
    if(!silent) emit changed();
}

void MediaInfo::setFFmpegOptions(QList<QStringList> options, bool silent )
{
    _ffmpegOptions = options;
    if(!silent) emit changed();
}

void MediaInfo::setVideo(bool video, bool silent )
{
    _video = video;
    if(!silent) emit changed();
}

void MediaInfo::setAudio(bool audio, bool silent )
{
    _audio = audio;
    if(!silent) emit changed();
}

void MediaInfo::setMuxer(FFMuxer *muxer, bool silent )
{
    _muxer = muxer;
    if (muxer->isSequence())
    {
        _audio = false;
        _imageSequence = true;
        loadSequence();
    }
    else
    {
        _audio = muxer->isAudio();
        _video = muxer->isVideo();
        _imageSequence = false;
    }
    if(!silent) emit changed();
}

void MediaInfo::setMuxer(QString name, bool silent )
{
    setMuxer( _ffmpeg->muxer( name ), silent);
}

void MediaInfo::addFFmpegOption(QStringList option, bool silent )
{
    _ffmpegOptions << option;
    if(!silent) emit changed();
}

void MediaInfo::removeFFmpegOptions(QString optionName, bool silent )
{
    for(int i = _ffmpegOptions.count()-1 ; i >= 0 ; i--)
    {
        if (_ffmpegOptions[i][0] == optionName)
        {
            _ffmpegOptions.removeAt(i);
        }
    }
    if(!silent) emit changed();
}

void MediaInfo::clearFFmpegOptions(bool silent )
{
    _ffmpegOptions.clear();
    if(!silent) emit changed();
}

void MediaInfo::setAlpha( bool alpha, bool silent )
{
    //select a pixfmt which has an alpha, the closest to the current one (or default)
    FFPixFormat *pf = _pixFormat;
    FFCodec *vc = _videoCodec;
    if (vc == nullptr) vc = defaultVideoCodec();
    if (vc == nullptr) return;

    if ( pf == nullptr ) pf = vc->defaultPixFormat();
    if ( pf == nullptr && vc->pixFormats().count() > 0) pf = vc->pixFormats()[0];
    if ( pf == nullptr ) return;
    if ( pf->hasAlpha() != alpha )
    {
        //find the closest one with(out) alpha
        foreach ( FFPixFormat *p, vc->pixFormats() )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() && pf->isOutput() )
                    if (p->isInput() && pf->isInput())
                        if (p->colorSpace() == pf->colorSpace())
                            if ( (alpha && p->numComponents()+1 == pf->numComponents()) || (!alpha && p->numComponents() == pf->numComponents()+1) )
                                if( p->bitsPerPixel() == pf->bitsPerPixel() )
                                {
                                    _pixFormat = p;
                                    break;
                                }
        }
        foreach ( FFPixFormat *p, vc->pixFormats() )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() && pf->isOutput() )
                    if (p->isInput() && pf->isInput())
                        if (p->colorSpace() == pf->colorSpace())
                            if ( (alpha && p->numComponents()+1 == pf->numComponents()) || (!alpha && p->numComponents() == pf->numComponents()+1) )
                            {
                                _pixFormat = p;
                                break;
                            }

        }
        foreach ( FFPixFormat *p, vc->pixFormats() )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() && pf->isOutput() )
                    if (p->isInput() && pf->isInput())
                        if (p->colorSpace() == pf->colorSpace())
                        {
                            _pixFormat = p;
                            break;
                        }
        }
        foreach ( FFPixFormat *p, vc->pixFormats() )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() && pf->isOutput() )
                    if (p->isInput() && pf->isInput())
                    {
                        _pixFormat = p;
                        break;
                    }
        }
        foreach ( FFPixFormat *p, vc->pixFormats() )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() && pf->isOutput() )
                {
                    _pixFormat = p;
                    break;
                }
        }
        foreach ( FFPixFormat *p, vc->pixFormats() )
        {
            if ( p->hasAlpha() == alpha )
            {
                _pixFormat = p;
                break;
            }
        }
    }

    if(!silent) emit changed();
}

void MediaInfo::setColorRange(const QString &colorRange, bool silent)
{
    _colorRange = colorRange;
    if (!silent) emit changed();
}

void MediaInfo::addAudioStream(AudioInfo *stream, bool silent)
{
    _audioStreams << stream;
    if (!silent) emit changed();
}

void MediaInfo::addVideoStream(VideoInfo *stream, bool silent)
{
    _videoStreams << stream;
    if (!silent) emit changed();
}

void MediaInfo::addMap(int mediaId, int streamId, bool silent)
{
    _maps << StreamReference(mediaId, streamId);
    if (!silent) emit changed();
}

void MediaInfo::removeMap(int index, bool silent)
{
    if (index >= 0 && index < _maps.count())
    {
        _maps.removeAt(index);
        if (!silent) emit changed();
    }
}

void MediaInfo::setColorPrimaries(const QString &colorPrimaries, bool silent)
{
    _colorPrimaries = colorPrimaries;
    if (!silent) emit changed();
}

void MediaInfo::setColorTRC(const QString &colorTRC, bool silent)
{
    _colorTRC = colorTRC;
    if (!silent) emit changed();
}

void MediaInfo::setColorSpace(const QString &colorSpace, bool silent)
{
    _colorSpace = colorSpace;
    if (!silent) emit changed();
}

void MediaInfo::setCacheDir(QTemporaryDir *aepTempDir, bool silent )
{
    _cacheDir = aepTempDir;
    if(!silent) emit changed();
}

void MediaInfo::setAeUseRQueue(bool aeUseRQueue, bool silent )
{
    _aeUseRQueue = aeUseRQueue;
    if(!silent) emit changed();
}

void MediaInfo::setAepRqindex(int aepRqindex, bool silent )
{
    _aepRqindex = aepRqindex;
    if(!silent) emit changed();
}

void MediaInfo::setAepNumThreads(int aepNumThreads, bool silent )
{
    _aepNumThreads = aepNumThreads;
    if(!silent) emit changed();
}

void MediaInfo::setAepCompName(const QString &aepCompName, bool silent )
{
    _aepCompName = aepCompName;
    if(!silent) emit changed();
}

void MediaInfo::setPixFormat(FFPixFormat *pixFormat, bool silent )
{
    bool alpha = hasAlpha();
    _pixFormat = pixFormat;
    if (_pixFormat == nullptr)
    {
        setAlpha(alpha);
    }
    if(!silent) emit changed();
}

void MediaInfo::setPixFormat(QString name, bool silent )
{
    setPixFormat( _ffmpeg->pixFormat(name), silent );
}

void MediaInfo::setFrames(const QStringList &frames, bool silent )
{
    _frames = frames;
    if(!silent) emit changed();
}

void MediaInfo::setStartNumber(int startNumber, bool silent )
{
    _startNumber = startNumber;
    if(!silent) emit changed();
}

void MediaInfo::setAep(bool isAep, bool silent )
{
    _isAep = isAep;
    if(!silent) emit changed();
}

void MediaInfo::setVideoQuality(int quality, bool silent )
{
    _videoQuality = quality;
    if(!silent) emit changed();
}

void MediaInfo::setVideoProfile(int profile, bool silent )
{
    _videoProfile = profile;
    if(!silent) emit changed();
}

void MediaInfo::setLoop(int loop, bool silent )
{
    _loop = loop;
    if(!silent) emit changed();
}

void MediaInfo::setPremultipliedAlpha(bool premultipliedAlpha, bool silent )
{
    _premultipliedAlpha = premultipliedAlpha;
    if(!silent) emit changed();
}

// GETTERS

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

FFCodec *MediaInfo::defaultVideoCodec() const
{
    if (_muxer != nullptr) return _muxer->defaultVideoCodec();
    return nullptr;
}

FFCodec *MediaInfo::audioCodec()
{
    return _audioCodec;
}

FFCodec *MediaInfo::defaultAudioCodec() const
{
    if (_muxer != nullptr) return _muxer->defaultAudioCodec();
    return nullptr;
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

QString MediaInfo::colorPrimaries() const
{
    return _colorPrimaries;
}

QString MediaInfo::colorTRC() const
{
    return _colorTRC;
}

QString MediaInfo::colorSpace() const
{
    return _colorSpace;
}

QString MediaInfo::colorRange() const
{
    return _colorRange;
}

FFmpeg *MediaInfo::getFfmpeg() const
{
    return _ffmpeg;
}

QList<StreamReference> MediaInfo::maps() const
{
    return _maps;
}

QList<VideoInfo *> MediaInfo::videoStreams() const
{
    return _videoStreams;
}

QList<AudioInfo *> MediaInfo::audioStreams() const
{
    return _audioStreams;
}

bool MediaInfo::hasAlpha() const
{
    //Check in streams if there is alpha
    for (int i = 0; i < _videoStreams.count(); i++)
    {
        if ( _videoStreams[i]->hasAlpha() ) return true;
    }

    // check with current pixFmt
    if (_pixFormat != nullptr) return _pixFormat->hasAlpha();

    // check with current video codec (or default one)
    FFCodec *vc = _videoCodec;
    if (vc == nullptr) vc = defaultVideoCodec();
    if (vc == nullptr ) return _isAep;
    FFPixFormat *pf = vc->defaultPixFormat();
    if (pf != nullptr) return pf->hasAlpha();

    return _isAep;
}

bool MediaInfo::canHaveAlpha() const
{
    //Check in streams if there is alpha
    for (int i = 0; i < _videoStreams.count(); i++)
    {
        if ( _videoStreams[i]->canHaveAlpha() ) return true;
    }

    //Check with current or default video codec
    FFCodec *vc = _videoCodec;
    if (vc == nullptr) vc = defaultVideoCodec();
    if (vc == nullptr) return false;

    foreach (FFPixFormat *pf, vc->pixFormats())
    {
        if (pf->hasAlpha()) return true;
    }

    return _isAep;
}

bool MediaInfo::copyVideo() const
{
    FFCodec *vc = _videoCodec;
    if (vc == nullptr) return false;
    return vc->name() == "copy";
}

bool MediaInfo::copyAudio() const
{
    FFCodec *ac = _audioCodec;
    if (ac == nullptr) return false;
    return ac->name() == "copy";
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

bool MediaInfo::aeUseRQueue() const
{
    return _aeUseRQueue;
}

int MediaInfo::aepRqindex() const
{
    return _aepRqindex;
}

int MediaInfo::aepNumThreads() const
{
    return _aepNumThreads;
}

QString MediaInfo::aepCompName() const
{
    return _aepCompName;
}

bool MediaInfo::isAep() const
{
    return _isAep;
}

bool MediaInfo::premultipliedAlpha() const
{
    return _premultipliedAlpha;
}

FFPixFormat *MediaInfo::pixFormat()
{
    return _pixFormat;
}

FFPixFormat *MediaInfo::defaultPixFormat() const
{
    FFCodec *c = nullptr;

    //get first stream codec
    if ( _videoStreams.count() > 0) c = _videoStreams[0]->codec();

    //get current or default
    if ( c == nullptr )
    {
        c = _videoCodec;
        if ( c == nullptr ) c = defaultVideoCodec();
        if ( c == nullptr ) return nullptr;
    }

    FFPixFormat *pf = c->defaultPixFormat();
    if ( pf == nullptr && c->name() == "h264") pf = _ffmpeg->pixFormat("yuv420p");
    return pf;
}

QStringList MediaInfo::frames() const
{
    return _frames;
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

    //get all files of same type in folder
    QDir containingDir(dirPath);
    QFileInfoList files = containingDir.entryInfoList(QStringList("*." + extension),QDir::Files);

    //find where is the base file
    int baseIndex = -1;
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
    if (baseIndex >= 0)
    {
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

int MediaInfo::videoQuality() const
{
    return _videoQuality;
}

int MediaInfo::videoProfile() const
{
    return _videoProfile;
}

int MediaInfo::loop() const
{
    return _loop;
}
