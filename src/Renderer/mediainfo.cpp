#include "mediainfo.h"

MediaInfo::MediaInfo( FFmpeg *ffmpeg, QObject *parent ) : QObject(parent)
{
    _ffmpeg = ffmpeg;
    _id = -1;
    reInit();
}

MediaInfo::MediaInfo( FFmpeg *ffmpeg, QFileInfo mediaFile, QObject *parent ) : QObject(parent)
{
    _ffmpeg = ffmpeg;
    _id = -1;
    if ( mediaFile.suffix() == "dffp" ) loadPreset( mediaFile );
    else update( mediaFile );
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
    _frames.clear();
    _loop = -1;
    _startNumber = 0;
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

void MediaInfo::update(QFileInfo mediaFile, bool silent)
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
        }

        //test video stream
        match = reVideoStream.match(info);
        if (match.hasMatch())
        {
            //add the stream
            VideoInfo *stream = new VideoInfo(_ffmpeg);
            stream->setId( match.captured(1).toInt() );
            stream->setLanguage( (match.captured(2)) );

            QString codec = match.captured(3).left( match.captured(3).indexOf("(") );
            FFCodec *c = _ffmpeg->videoEncoder( codec );
            if (c == nullptr ) c = _ffmpeg->videoDecoder( codec );
            if (c == nullptr ) c = new FFCodec(codec);
            stream->setCodec( c );

            QString pixFormat = match.captured(4).left( match.captured(4).indexOf("(") );
            FFPixFormat *pf = _ffmpeg->pixFormat( pixFormat );
            if ( pf == nullptr ) pf = new FFPixFormat( pixFormat );
            stream->setPixFormat( pf );

            stream->setWidth( match.captured(5).toInt() );
            stream->setHeight( match.captured(6).toInt() );
            if ( match.captured(8) != "" ) stream->setPixAspect( match.captured(7).toFloat() / match.captured(8).toFloat() );

            stream->setBitrate( match.captured(11).toInt()*1024 );
            stream->setFramerate( match.captured(12).toDouble() );

            if ( int( stream->framerate() ) == 0 ) stream->setFramerate( 24 );

            addVideoStream( stream );
            continue;
        }

        //test audio stream
        match = reAudioStream.match(info);
        if (match.hasMatch())
        {
            //add the stream
            AudioInfo *stream = new AudioInfo(_ffmpeg);
            stream->setId( match.captured(1).toInt() );
            stream->setLanguage( match.captured(2) );

            QString codec = match.captured(3).left( match.captured(3).indexOf("(") );
            FFCodec *c = _ffmpeg->audioEncoder( codec );
            if (c == nullptr ) c = _ffmpeg->audioDecoder( codec );
            if ( c == nullptr ) c = new FFCodec( codec );
            stream->setCodec(c);

            stream->setSamplingRate( match.captured(4).toInt() );

            stream->setChannels( match.captured(5) );

            stream->setBitrate( match.captured(6).toInt()*1024 );

            addAudioStream( stream );
        }
    }

    if (_muxer->isSequence())
    {
        loadSequence();
    }

    if(!silent) emit changed();
}

void MediaInfo::copyFrom(MediaInfo *other, bool updateFilename, bool silent)
{
    // GENERAL
    if (updateFilename) _fileName = other->fileName();
    _extensions = other->extensions();
    _muxer = other->muxer();
    _duration = other->duration();
    _size = other->size();
    _bitrate = other->bitrate();
    _frames = other->frames();
    _loop = other->loop();
    _startNumber = other->startNumber();
    // STREAMS
    qDeleteAll(_videoStreams);
    _videoStreams.clear();
    qDeleteAll(_audioStreams);
    _audioStreams.clear();
    foreach(VideoInfo *stream, other->videoStreams())
    {
        VideoInfo *st = new VideoInfo(_ffmpeg, this);
        st->copyFrom(stream, true);
        _videoStreams << st;
    }
    foreach(AudioInfo *stream, other->audioStreams())
    {
        AudioInfo *st = new AudioInfo(_ffmpeg, this);
        st->copyFrom(stream, true);
        _audioStreams << st;
    }
    // MAPS
    _maps = other->maps();
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
    else if ( _muxer != nullptr && _muxer->isSequence() )
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
        if ( hasVideo() ) mediaInfoString += "yes";
        else mediaInfoString += "no";
        mediaInfoString += "\nContains audio: ";
        if ( hasAudio() ) mediaInfoString += "yes";
        else mediaInfoString += "no";
        mediaInfoString += "\nImage sequence: ";
        if ( _muxer != nullptr && _muxer->isSequence() ) mediaInfoString += "yes";
        else mediaInfoString += "no";

        for ( int i = 0; i < _videoStreams.count(); i++)
        {
            VideoInfo *s = _videoStreams[i];

            mediaInfoString += "\n\nVideo stream";
            if (s->id() >= 0) mediaInfoString += " #" + QString::number( s->id() ) + ":";

            if ( s->language()->name() != "") mediaInfoString += "\nVideo language: " + s->language()->prettyName();
            mediaInfoString += "\nVideo codec: ";
            FFCodec *vc = s->codec();
            if (vc == nullptr) vc = defaultVideoCodec();
            if (vc != nullptr)
            {
                mediaInfoString += vc->prettyName();
            }

            if ( vc->name() == "copy" ) continue;

            if (s->width() !=0 || s->height() != 0 ) mediaInfoString += "\nResolution: " + QString::number( s->width() ) + "x" + QString::number( s->height() );
            if (s->aspect() != 0 ) mediaInfoString += "\nVideo Aspect: " + QString::number( int( s->aspect()*100+0.5 ) / 100.0) + ":1";
            if (s->framerate() != 0 ) mediaInfoString += "\nFramerate: " + QString::number(s->framerate()) + " fps";
            if (s->profile() != nullptr) if (s->profile()->name() != "") mediaInfoString += "\nProfile: " + s->profile()->prettyName();
            if (s->level() != "") mediaInfoString += "\nLevel: " + s->level();
            qint64 bitrate = s->bitrate();
            if (bitrate != 0) mediaInfoString += "\nBitrate: " + MediaUtils::bitrateString(bitrate);
            if (s->quality() >= 0) mediaInfoString += "\nQuality: " + QString::number(s->quality()) + "%";
            mediaInfoString += "\nPixel Aspect: " + QString::number( int(s->pixAspect()*100+0.5)/ 100.0) + ":1";
            if (s->canHaveAlpha()) mediaInfoString += "\nCan have alpha: yes";
            else if (s->codec() == nullptr && canHaveAlpha()) mediaInfoString += "\nCan have alpha: yes";
            else mediaInfoString += "\nCan have alpha: no";
            FFPixFormat *pf = s->pixFormat();
            if ( pf == nullptr ) pf = defaultPixFormat();
            if (pf != nullptr)
            {
                if (pf->hasAlpha())
                {
                    mediaInfoString += "\nAlpha: yes";
                    if (!s->premultipliedAlpha()) mediaInfoString += " (Unmultiply)";
                }
                else mediaInfoString += "\nAlpha: no";
                mediaInfoString += "\nPixel Format: " + pf->prettyName();
            }
            if ( s->colorTRC()->name() == "iec61966_2_1" && s->colorPrimaries()->name() == "bt709" && s->colorSpace()->name() == "rgb")
            {
                mediaInfoString += "\nColor profile: sRGB";
            }
            else if ( s->colorTRC()->name() == "bt709" && s->colorPrimaries()->name() == "bt709" && s->colorSpace()->name() == "bt709")
            {
                mediaInfoString += "\nColor profile: HD video, BT.709";
            }
            else if ( s->colorTRC()->name() == "bt2020_10" && s->colorPrimaries()->name() == "bt2020" && s->colorSpace()->name() == "bt2020_cl")
            {
                mediaInfoString += "\nColor profile: UHD video, BT.2020 10 bits";
            }
            else if ( s->colorTRC()->name() == "bt2020_12" && s->colorPrimaries()->name() == "bt2020" && s->colorSpace()->name() == "bt2020_cl")
            {
                mediaInfoString += "\nColor profile: UHD HDR video, BT.2020 12 bits";
            }
            else
            {
                if ( s->colorSpace()->name() != "") mediaInfoString += "\nColor space: " + s->colorSpace()->prettyName();
                if ( s->colorPrimaries()->name() != "") mediaInfoString += "\nColor primaries: " + s->colorPrimaries()->prettyName();
                if ( s->colorTRC()->name() != "") mediaInfoString += "\nColor transfer function: " + s->colorTRC()->prettyName();
            }
            if ( s->colorRange()->name() != "" ) mediaInfoString += "\nColor range: " + s->colorRange()->prettyName();
        }

        for ( int i = 0; i < _audioStreams.count(); i++)
        {
            AudioInfo *s = _audioStreams[i];

            mediaInfoString += "\n\nAudio stream";
            if (s->id() >= 0) mediaInfoString += " #" + QString::number( s->id() ) + ":";

            if ( s->language()->name() != "") mediaInfoString += "\nAudio language: " + s->language()->prettyName();
            mediaInfoString += "\nAudio codec: ";
            FFCodec *ac = s->codec();
            if (ac == nullptr) ac = defaultAudioCodec();
            if(ac != nullptr)
            {
                mediaInfoString += ac->prettyName();
            }

            if ( ac->name() == "copy" ) continue;

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
    if (mainObj.value("dume").isUndefined()) return;

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
    QJsonArray vStreams = mediaObj.value("videoStreams").toArray();

    foreach( QJsonValue stream, vStreams )
    {
        VideoInfo *s = new VideoInfo(stream.toObject(), _ffmpeg); 
        addVideoStream( s, true);
    }

    //audio
    QJsonArray aStreams = mediaObj.value("audioStreams").toArray();
    foreach( QJsonValue stream, aStreams )
    {
        AudioInfo *s = new AudioInfo(stream.toObject(), _ffmpeg);
        addAudioStream( s, true);
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
    if (_muxer != nullptr) mediaObj.insert("muxer", _muxer->toJson());

    //video streams
    QJsonArray vStreams = QJsonArray();
    foreach(VideoInfo *stream, _videoStreams)
    {
        vStreams.append( stream->toJson() );
    }
    mediaObj.insert("videoStreams", vStreams );

    //audio streams
    QJsonArray aStreams = QJsonArray();
    foreach(AudioInfo *stream, _audioStreams)
    {
        aStreams.append( stream->toJson() );
    }
    mediaObj.insert("audioStreams", aStreams );

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

void MediaInfo::setMuxer(FFMuxer *muxer, bool silent )
{
    qDeleteAll( _audioStreams );
    _audioStreams.clear();
    qDeleteAll( _videoStreams );
    _videoStreams.clear();
    _muxer = muxer;
    if (muxer != nullptr && muxer->isSequence()) loadSequence();

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

void MediaInfo::addAudioStream(AudioInfo *stream, bool silent)
{
    if (_audioStreams.contains(stream)) return;
    _audioStreams << stream;
    stream->setParent(this);
    connect(stream, SIGNAL(changed()), this, SLOT(streamChanged()));
    if (!silent) emit changed();
}

AudioInfo *MediaInfo::takeAudioStream(int index, bool silent)
{
    AudioInfo *stream = _audioStreams.takeAt(index);
    disconnect(stream, SIGNAL(changed()), this, SLOT(changed()));
    if (!silent) emit changed();
    return stream;
}

void MediaInfo::clearAudioStreams(bool silent)
{
    qDeleteAll(_audioStreams);
    _audioStreams.clear();
    if (!silent) emit changed();
}

void MediaInfo::addVideoStream(VideoInfo *stream, bool silent)
{
    if ( _videoStreams.contains(stream) ) return;
    _videoStreams << stream;
    stream->setParent(this);
    connect(stream, SIGNAL(changed()), this, SLOT(streamChanged()));
    if (!silent) emit changed();
}

VideoInfo *MediaInfo::takeVideoStream(int index, bool silent)
{
    VideoInfo *stream = _videoStreams.takeAt(index);
    disconnect(stream, SIGNAL(changed()), this, SLOT(changed()));
    if (!silent) emit changed();
    return stream;
}

void MediaInfo::clearVideoStreams(bool silent)
{
    qDeleteAll(_videoStreams);
    _videoStreams.clear();
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

void MediaInfo::removeAllMaps(bool silent)
{
    _maps.clear();
    if (!silent) emit changed();
}

void MediaInfo::setMap(int mapIndex, int mediaId, int streamId, bool silent)
{
    if (mapIndex >= 0 && mapIndex < _maps.count())
    {
        _maps[mapIndex].setMediaId(mediaId);
        _maps[mapIndex].setStreamId(streamId);
    }
    if (!silent) emit changed();
}

void MediaInfo::setMapMedia(int mapIndex, int mediaId, bool silent)
{
    if (mapIndex >= 0 && mapIndex < _maps.count())
    {
        _maps[mapIndex].setMediaId(mediaId);
    }
    if (!silent) emit changed();
}

void MediaInfo::setMapStream(int mapIndex, int streamId, bool silent)
{
    if (mapIndex >= 0 && mapIndex < _maps.count())
    {
        _maps[mapIndex].setStreamId(streamId);
    }
    if (!silent) emit changed();
}

void MediaInfo::setVideoQuality(int value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setQuality(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setQuality(value, silent);
}

void MediaInfo::setVideoProfile(QString value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setProfile(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setProfile(value, silent);
}

void MediaInfo::setVideoProfile(FFProfile *value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setProfile(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setProfile(value, silent);
}

void MediaInfo::setVideoLevel(QString value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setLevel(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setLevel(value, silent);
}

void MediaInfo::setPixAspect(float value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setPixAspect(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setPixAspect(value, silent);
}

void MediaInfo::setPixFormat(QString value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setPixFormat(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setPixFormat(value, silent);
}

void MediaInfo::setPixFormat(FFPixFormat *value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setPixFormat(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setPixFormat(value, silent);
}

void MediaInfo::setVideoBitrate(qint64 value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setBitrate(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setBitrate(value, silent);
}

void MediaInfo::setFramerate(double value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setFramerate(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setFramerate(value, silent);
}

void MediaInfo::setHeight(int value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setHeight(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setHeight(value, silent);
}

void MediaInfo::setWidth(int value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setWidth(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setWidth(value, silent);
}

void MediaInfo::setVideoCodec(QString value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setCodec(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setCodec(value, silent);
}

void MediaInfo::setVideoCodec(FFCodec *value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setCodec(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setCodec(value, silent);
}

void MediaInfo::setVideoLanguage(QString value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setLanguage(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setLanguage(value, silent);
}

void MediaInfo::setColorPrimaries(QString value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setColorPrimaries(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setColorPrimaries(value, silent);
}

void MediaInfo::setColorTRC(QString value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setColorTRC(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setColorTRC(value, silent);
}

void MediaInfo::setColorSpace(QString value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setColorSpace(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setColorSpace(value, silent);
}

void MediaInfo::setColorRange(QString value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setColorRange(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setColorRange(value, silent);
}

void MediaInfo::setPremultipliedAlpha(bool value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setPremultipliedAlpha(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setPremultipliedAlpha(value, silent);
}

void MediaInfo::setSamplingRate(int value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setSamplingRate(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _audioStreams[id]->setSamplingRate(value, silent);
}

void MediaInfo::setChannels(QString value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setChannels(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _audioStreams[id]->setChannels(value, silent);
}

void MediaInfo::setAudioBitrate(qint64 value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setBitrate(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _audioStreams[id]->setBitrate(value, silent);
}

void MediaInfo::setAudioCodec(QString value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setCodec(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _audioStreams[id]->setCodec(value, silent);
}

void MediaInfo::setAudioCodec(FFCodec *value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setCodec(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _audioStreams[id]->setCodec(value, silent);
}

void MediaInfo::setAudioLanguage(QString value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setLanguage(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _audioStreams[id]->setLanguage(value, silent);
}

void MediaInfo::streamChanged()
{
    emit changed();
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

void MediaInfo::setLoop(int loop, bool silent )
{
    _loop = loop;
    if(!silent) emit changed();
}

// GETTERS

double MediaInfo::duration()
{
    return _duration;
}

QString MediaInfo::fileName()
{
    return _fileName;
}

FFCodec *MediaInfo::defaultVideoCodec() const
{
    if (_muxer != nullptr) return _muxer->defaultVideoCodec();
    return nullptr;
}

FFCodec *MediaInfo::defaultAudioCodec() const
{
    if (_muxer != nullptr) return _muxer->defaultAudioCodec();
    return nullptr;
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
    return _videoStreams.count() > 0;
}

bool MediaInfo::hasAudio()
{
    return _audioStreams.count() > 0;
}

bool MediaInfo::isSequence()
{
    if (_muxer == nullptr) return false;
    return _muxer->isSequence();
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

FFmpeg *MediaInfo::getFfmpeg() const
{
    return _ffmpeg;
}

int MediaInfo::id() const
{
    return _id;
}

void MediaInfo::setId(int value)
{
    _id = value;
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

bool MediaInfo::hasAlpha()
{
    if (isAep()) return true;
    if (!hasVideo()) return false;

    //Check in streams if there is alpha
    for (int i = 0; i < _videoStreams.count(); i++)
    {
        if ( _videoStreams[i]->hasAlpha() ) return true;
    }

    return false;
}

bool MediaInfo::canHaveAlpha() const
{
    if (isAep()) return true;

    //Check in streams if there is alpha
    for (int i = 0; i < _videoStreams.count(); i++)
    {
        if ( _videoStreams[i]->canHaveAlpha() ) return true;
    }

    //Check with default video codec
    FFCodec *vc = defaultVideoCodec();
    if (vc == nullptr) return false;

    foreach (FFPixFormat *pf, vc->pixFormats())
    {
        if (pf->hasAlpha()) return true;
    }

    return false;
}

void MediaInfo::setAlpha(bool alpha, bool silent)
{
    FFCodec *vc = defaultVideoCodec();
    FFPixFormat *pf = nullptr;
    if (vc != nullptr) pf = vc->defaultPixFormat(alpha);

    foreach( VideoInfo *stream, _videoStreams)
    {
        bool ok = stream->setAlpha(alpha, silent);
        if (ok) continue;
        stream->setPixFormat( pf );
    }
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

FFPixFormat *MediaInfo::defaultPixFormat() const
{
    FFCodec *c = defaultVideoCodec();
    if (c == nullptr) return nullptr;
    return c->defaultPixFormat();
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

int MediaInfo::loop() const
{
    return _loop;
}
