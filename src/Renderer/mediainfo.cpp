#include "mediainfo.h"

MediaInfo::MediaInfo( QObject *parent ) : QObject(parent)
{
    _id = -1;
    _outputMedia = false;
    reInit();
}

MediaInfo::MediaInfo(QFileInfo mediaFile, QObject *parent ) : QObject(parent)
{
    _id = -1;
    _outputMedia = false;
    if ( mediaFile.suffix() == "dffp" ) loadPreset( mediaFile );
    else update( mediaFile );
}

void MediaInfo::reInit(bool removeFileName, bool silent)
{
    // GENERAL
    if (removeFileName) _fileName = "";
    _extensions.clear();
    _muxer = FFmpeg::instance()->muxer("");
    _duration = 0.0;
    _size = 0;
    _bitrate = 0;
    _frames.clear();
    _missingFrames.clear();
    _emptyFrames.clear();
    _loop = -1;
    _info = "";
    _inPoint = 0.0;
    _outPoint = 0.0;
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

    _muxer = FFmpeg::instance()->muxer(extension);

    if (!mediaFile.exists())
    {
        if (!silent) emit changed();
        return;
    }

    QString ffmpegOutput = FFmpeg::instance()->analyseMedia( mediaFile.absoluteFilePath() );

    QStringList infos = ffmpegOutput.split("\n");

    //regexes to get infos
    QRegularExpression reInput = RegExUtils::getRegularExpression("ffmpeg input");
    QRegularExpression reVideoStream = RegExUtils::getRegularExpression("ffmpeg video stream");
    QRegularExpression reAudioStream = RegExUtils::getRegularExpression("ffmpeg audio stream");
    QRegularExpression reDuration = RegExUtils::getRegularExpression("ffmpeg duration");
    QRegularExpression reResolution = RegExUtils::getRegularExpression("ffmpeg resolution");
    QRegularExpression reFPS = RegExUtils::getRegularExpression("ffmpeg fps");
    QRegularExpression reBitrate = RegExUtils::getRegularExpression("ffmpeg bitrate");
    QRegularExpression reAspect = RegExUtils::getRegularExpression("ffmpeg aspect");

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
            VideoInfo *stream = new VideoInfo();
            stream->setId( match.captured(1).toInt() );
            stream->setLanguage( (match.captured(2)) );

            QString codec = match.captured(3).left( match.captured(3).indexOf("(") );
            FFCodec *c = FFmpeg::instance()->videoEncoder( codec );
            if (c->name() == "" ) c = FFmpeg::instance()->videoDecoder( codec );
            if (c->name() == "" ) c = _muxer->defaultVideoCodec();
            stream->setCodec( c );

            QString pixFormat = match.captured(4).left( match.captured(4).indexOf("(") );
            FFPixFormat *pf = FFmpeg::instance()->pixFormat( pixFormat );
            stream->setPixFormat( pf );

            QRegularExpressionMatch matchRes = reResolution.match( info );
            if (matchRes.hasMatch())
            {
                stream->setWidth( matchRes.captured(1).toInt() );
                stream->setHeight( matchRes.captured(2).toInt() );
            }

            QRegularExpressionMatch matchAspect = reAspect.match( info );
            if (matchAspect.hasMatch())
            {
                stream->setPixAspect( matchAspect.captured(1).toFloat() / matchAspect.captured(2).toFloat() );
            }

            QRegularExpressionMatch matchBitrate = reBitrate.match( info );
            if (matchBitrate.hasMatch())
            {
                stream->setBitrate( matchBitrate.captured(1).toInt()*1024 );
            }
            QRegularExpressionMatch matchFPS = reFPS.match( info );
            if (matchFPS.hasMatch())
            {
                stream->setFramerate( matchFPS.captured(1).toDouble() );
            }

            addVideoStream( stream, silent );
            continue;
        }

        //test audio stream
        match = reAudioStream.match(info);
        if (match.hasMatch())
        {
            //add the stream
            AudioInfo *stream = new AudioInfo();
            stream->setId( match.captured(1).toInt() );
            stream->setLanguage( match.captured(2) );

            QString codec = match.captured(3).left( match.captured(3).indexOf("(") );
            FFCodec *c = FFmpeg::instance()->audioEncoder( codec );
            if (c->name() == "" ) c = FFmpeg::instance()->audioDecoder( codec );
            if (c->name() == "" ) c = _muxer->defaultAudioCodec();
            stream->setCodec(c);

            stream->setSamplingRate( match.captured(4).toInt() );

            stream->setChannels( match.captured(5) );

            stream->setSampleFormat(match.captured(6));

            QRegularExpressionMatch matchBitrate = reBitrate.match( info );
            if (matchBitrate.hasMatch())
            {
                stream->setBitrate( matchBitrate.captured(1).toInt()*1024 );
            }

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
    _inPoint = other->inPoint();
    _outPoint = other->outPoint();
    // STREAMS
    qDeleteAll(_videoStreams);
    _videoStreams.clear();
    qDeleteAll(_audioStreams);
    _audioStreams.clear();
    foreach(VideoInfo *stream, other->videoStreams())
    {
        VideoInfo *st = new VideoInfo(this);
        st->copyFrom(stream, true);
        _videoStreams << st;
    }
    foreach(AudioInfo *stream, other->audioStreams())
    {
        AudioInfo *st = new AudioInfo(this);
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
    if (_info != "" && !_outputMedia) mediaInfoString += _info + "\n";

    if (_muxer->isSequence())
    {
        if (_missingFrames.count() > 0)
        {
            mediaInfoString += "Missing Frames in the sequence:\n";
            foreach(int f, _missingFrames)
            {
                mediaInfoString += QString::number(f) + "\n";
            }
            mediaInfoString += "\n";
        }

        if (_emptyFrames.count() > 0)
        {
            mediaInfoString += "Some frames of the sequence are empty (< 10 Bytes):\n";
            foreach(QFileInfo f, _emptyFrames)
            {
                mediaInfoString += f.completeBaseName() + "\n";
            }
            mediaInfoString += "\n";
        }
    }


    QFileInfo fileInfo(_fileName);

    if ( _isAep )
    {
        if (fileInfo.suffix() == "aep") mediaInfoString += "After Effects project.\n";
        if (fileInfo.suffix() == "aet") mediaInfoString += "After Effects template.\n";
        if (fileInfo.suffix() == "aepx") mediaInfoString += "After Effects XML project.\n";
    }

    if ( _muxer->name() != "" )
    {
        mediaInfoString += "File format: " + _muxer->prettyName();
    }
    else
    {
        mediaInfoString += "Containers: " + _extensions.join(",");
    }


    if (_duration != 0.0 && !_muxer->isSequence())
    {
        mediaInfoString += "\nDuration: " + MediaUtils::durationToTimecode(_duration);
    }
    else if ( _muxer->isSequence() )
    {
        mediaInfoString += "\nDuration: " + QString::number(  _frames.count() ) + " frames";
        if (_videoStreams.count() > 0)
        {
            double fr = _videoStreams[0]->framerate();
            if (fr == 0) fr = 24.0;
            mediaInfoString += "\nDuration @" +
                               QString::number(fr) +
                               "fps: " +
                               MediaUtils::durationToTimecode( _frames.count() / fr );
        }
        if (!_outputMedia)
        {
            //mediaInfoString += "\nStart Frame Number: " + QString::number( _startNumber );
            mediaInfoString += "\nEnd Frame Number: " + QString::number( _endNumber );
        }
    }

    if (_inPoint != 0.0) mediaInfoString += "\nIn point: " + MediaUtils::durationToTimecode(_inPoint);
    if (_outPoint != 0.0) mediaInfoString += "\nOut point: " + MediaUtils::durationToTimecode(_outPoint);

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
        if ( _muxer->isSequence() ) mediaInfoString += "yes";
        else mediaInfoString += "no";

        for ( int i = 0; i < _videoStreams.count(); i++)
        {
            VideoInfo *s = _videoStreams[i];
            mediaInfoString += "\n\n";
            mediaInfoString += s->getDescription(_outputMedia);
        }

        for ( int i = 0; i < _audioStreams.count(); i++)
        {
            AudioInfo *s = _audioStreams[i];

            mediaInfoString += "\n\nAudio stream";
            if (s->id() >= 0) mediaInfoString += " #" + QString::number( s->id() ) + ":";

            if ( s->language()->name() != "") mediaInfoString += "\nAudio language: " + s->language()->prettyName();
            mediaInfoString += "\nAudio codec: ";
            FFCodec *ac = s->codec();
            if (ac->name() == "") ac = defaultAudioCodec();
            mediaInfoString += ac->prettyName();

            if ( ac->name() == "copy" ) continue;

            if (s->samplingRate() != 0) mediaInfoString += "\nSampling rate: " + QString::number( s->samplingRate() ) + " Hz";
            if (s->sampleFormat()->name() != "") mediaInfoString += "\nSample Format (bit depth): " + s->sampleFormat()->prettyName();
            if (s->channels() != "")
            {
                mediaInfoString += "\nChannels: " + s->channels();
            }
            int abitrate = int( s->bitrate() );
            if (abitrate != 0) mediaInfoString += "\nBitrate: " + MediaUtils::bitrateString(abitrate);
        }
    }

    if (_loop == 0) mediaInfoString += "\n\nInfinite loops";
    else if (_loop > 0) mediaInfoString += "\n\nLoops: " + QString::number(_loop);

    //Custom params
    if (_ffmpegOptions.count() > 0)
    {
        mediaInfoString += "\n\nUsing FFmpeg custom options:";
        foreach(QStringList o, _ffmpegOptions)
        {
            mediaInfoString += "\n" + o[0];
            if (o.count() == 2) if (o[1] != "") mediaInfoString += " \"" + o[1] + "\"";
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

    qDebug() << "Valid JSON";

    QJsonObject mainObj = jsonDoc.object();
    if (mainObj.value("dume").isUndefined()) return;

    reInit(false, true);

    //load and create mediaInfo
    QJsonObject mediaObj = mainObj.value("dume").toObject();
    QString version = mediaObj.value("version").toString();
    //TODO Check version

    qDebug() << "IN/OUT points";

    //Time range
    setInPoint( mediaObj.value("inPoint").toDouble(0.0), true);
    setOutPoint( mediaObj.value("outPoint").toDouble(0.0), true);

    qDebug() << "Muxer";

    //muxer
    QJsonObject muxerObj = mediaObj.value("muxer").toObject();

    qDebug() << "Muxer is: " + muxerObj.value("name").toString("NOT FOUND!");

    setMuxer( muxerObj.value("name").toString(), true);
    setLoop( mediaObj.value("loop").toInt(), true );

    qDebug() << "Getting Video streams";

    //video
    QJsonArray vStreams = mediaObj.value("videoStreams").toArray();

    qDebug() << "Got " + QString::number(vStreams.count());

    foreach( QJsonValue stream, vStreams )
    {
        VideoInfo *s = new VideoInfo(stream.toObject());
        addVideoStream( s, true);
    }

    qDebug() << "Audio streams";

    //audio
    QJsonArray aStreams = mediaObj.value("audioStreams").toArray();
    foreach( QJsonValue stream, aStreams )
    {
        AudioInfo *s = new AudioInfo(stream.toObject());
        addAudioStream( s, true);
    }

    qDebug() << "Custom parameters";

    //options
    QJsonArray options = mediaObj.value("options").toArray();
    foreach(QJsonValue option,options)
    {
        QJsonObject optionObj = option.toObject();
        QStringList opt(optionObj.value("name").toString());
        opt << optionObj.value("value").toString();
        addFFmpegOption(opt, true );
    }

    qDebug() << "OK!";

    if(!silent) emit changed();

    qDebug() << presetFile.completeBaseName() + " Loaded.";
}

QString MediaInfo::exportPreset()
{
    QJsonObject mediaObj;
    mediaObj.insert("version", qApp->applicationVersion());

    //time range
    mediaObj.insert("inPoint", _inPoint);
    mediaObj.insert("outPoint", _outPoint);

    //muxer
    mediaObj.insert("muxer", _muxer->toJson());

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
    if ( _muxer->isSequence() ) loadSequence(silent);
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
    if ( muxer->isSequence()) loadSequence();

    if(!silent) emit changed();
}

void MediaInfo::setMuxer(QString name, bool silent )
{
    setMuxer( FFmpeg::instance()->muxer( name ), silent);
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

void MediaInfo::setStartNumber(int startNumber, int id, bool silent )
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setStartNumber(startNumber, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setStartNumber(startNumber, silent);
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

void MediaInfo::setVideoEncodingSpeed(int value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setEncodingSpeed(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setEncodingSpeed(value, silent);
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

void MediaInfo::setVideoProfile(FFBaseObject *value, int id, bool silent)
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

void MediaInfo::setVideoTuning(QString value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setTuning(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setTuning(value, silent);
}

void MediaInfo::setVideoBitrateType(QString type, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setBitrateType(type, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setBitrateType(type, silent);
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
    if (value == "") value = defaultVideoCodec()->name();
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setCodec(value, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setCodec(value, silent);
}

void MediaInfo::setVideoCodec(FFCodec *value, int id, bool silent)
{
    if (!hasVideo()) return;
    if (value->name() == "") value = defaultVideoCodec();
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

void MediaInfo::setColorProfile(QString profile, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setColorProfile(profile, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setColorProfile(profile, silent);
}

void MediaInfo::setColorConversionMode(MediaUtils::ColorConversionMode mode, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setColorConversionMode(mode, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setColorConversionMode(mode, silent);
}

void MediaInfo::setColorConversionMode(QString mode, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setColorConversionMode(mode, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setColorConversionMode(mode, silent);
}

void MediaInfo::setWorkingSpace(QString workingSpace, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setWorkingSpace(workingSpace, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setWorkingSpace(workingSpace, silent);
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

void MediaInfo::setCrop(int top, int bottom, int left, int right, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setCrop(top, bottom, left, right, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setCrop(top, bottom, left, right, silent);

}

void MediaInfo::setCrop(int width, int height, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setCrop(width, height, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setCrop(width, height, silent);
}

void MediaInfo::setCropUseSize(bool useSize, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setCropUseSize(useSize, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setCropUseSize(useSize, silent);
}

void MediaInfo::setLut(QString lut, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setLut(lut, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setLut(lut, silent);
}

void MediaInfo::setLut(FFLut *lut, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setLut(lut, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setLut(lut, silent);
}

void MediaInfo::setApplyLutOnOutputSpace(bool applyonOutput, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setApplyLutOnOutputSpace(applyonOutput, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setApplyLutOnOutputSpace(applyonOutput, silent);
}

void MediaInfo::setDeinterlace(bool deinterlace, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setDeinterlace(deinterlace, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setDeinterlace(deinterlace, silent);
}

void MediaInfo::setDeinterlaceParity(MediaUtils::DeinterlaceParity parity, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setDeinterlaceParity(parity, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setDeinterlaceParity(parity, silent);
}

void MediaInfo::setIntra(bool intra, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setIntra(intra, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setIntra(intra, silent);
}

void MediaInfo::setLossless(bool lossless, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setLossless(lossless, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setLossless(lossless, silent);
}

void MediaInfo::setVideoSpeed(float speed, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setSpeed(speed, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setSpeed(speed, silent);
}

void MediaInfo::setVideoSpeedInterpolationMode(MediaUtils::MotionInterpolationMode mode, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setSpeedInterpolationMode(mode, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setSpeedInterpolationMode(mode, silent);
}

void MediaInfo::setVideoSpeedEstimationMode(FFBaseObject *speedEstimationMode, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setSpeedEstimationMode(speedEstimationMode, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setSpeedEstimationMode(speedEstimationMode, silent);
}

void MediaInfo::setVideoSpeedEstimationMode(QString speedEstimationMode, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setSpeedEstimationMode(speedEstimationMode, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setSpeedEstimationMode(speedEstimationMode, silent);
}

void MediaInfo::setVideoSpeedAlgorithm(FFBaseObject *speedAlgorithm, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setSpeedAlgorithm(speedAlgorithm, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setSpeedAlgorithm(speedAlgorithm, silent);
}

void MediaInfo::setVideoSpeedAlgorithm(QString speedAlgorithm, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setSpeedAlgorithm(speedAlgorithm, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setSpeedAlgorithm(speedAlgorithm, silent);
}

void MediaInfo::setSceneDetection(bool sceneDetection, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setSceneDetection(sceneDetection, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setSceneDetection(sceneDetection, silent);
}

void MediaInfo::setResizeMode(MediaUtils::ResizeMode mode, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setResizeMode(mode, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setResizeMode(mode, silent);
}

void MediaInfo::setResizeMode(QString mode, int id, bool silent)
{
    setResizeMode( MediaUtils::ResizeModeFromString(mode), id, silent);
}

void MediaInfo::setResizeAlgorithm(FFBaseObject *algo, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setResizeAlgorithm(algo, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setResizeAlgorithm(algo, silent);
}

void MediaInfo::setResizeAlgorithm(QString algo, int id, bool silent)
{
    if (!hasVideo()) return;
    if (id < 0)
        foreach( VideoInfo *stream, _videoStreams)
            stream->setResizeAlgorithm(algo, silent);
    else if (id >= 0 && id < _videoStreams.count())
        _videoStreams[id]->setResizeAlgorithm(algo, silent);
}

void MediaInfo::setSamplingRate(int value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setSamplingRate(value, silent);
    else if (id >= 0 && id < _audioStreams.count())
        _audioStreams[id]->setSamplingRate(value, silent);
}

void MediaInfo::setChannels(QString value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setChannels(value, silent);
    else if (id >= 0 && id < _audioStreams.count())
        _audioStreams[id]->setChannels(value, silent);
}

void MediaInfo::setAudioBitrate(qint64 value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setBitrate(value, silent);
    else if (id >= 0 && id < _audioStreams.count())
        _audioStreams[id]->setBitrate(value, silent);
}

void MediaInfo::setAudioCodec(QString value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (value == "") value = defaultAudioCodec()->name();
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setCodec(value, silent);
    else if (id >= 0 && id < _audioStreams.count())
        _audioStreams[id]->setCodec(value, silent);
}

void MediaInfo::setAudioCodec(FFCodec *value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (value->name() == "") value = defaultAudioCodec();
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setCodec(value, silent);
    else if (id >= 0 && id < _audioStreams.count())
        _audioStreams[id]->setCodec(value, silent);
}

void MediaInfo::setAudioLanguage(QString value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setLanguage(value, silent);
    else if (id >= 0 && id < _audioStreams.count())
        _audioStreams[id]->setLanguage(value, silent);
}

void MediaInfo::setAudioSampleFormat(QString value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setSampleFormat(value, silent);
    else if (id >= 0 && id < _audioStreams.count())
        _audioStreams[id]->setSampleFormat(value, silent);
}

void MediaInfo::setAudioSampleFormat(FFSampleFormat *value, int id, bool silent)
{
    if (!hasAudio()) return;
    if (id < 0)
        foreach( AudioInfo *stream, _audioStreams)
            stream->setSampleFormat(value, silent);
    else if (id >= 0 && id < _audioStreams.count())
        _audioStreams[id]->setSampleFormat(value, silent);
}

void MediaInfo::streamChanged()
{
    emit changed();
}

bool MediaInfo::isOutputMedia() const
{
    return _outputMedia;
}

void MediaInfo::setOutputMedia(bool outputMedia)
{
    _outputMedia = outputMedia;
}

double MediaInfo::outPoint() const
{
    return _outPoint;
}

void MediaInfo::setOutPoint(double outPoint, bool silent)
{
    _outPoint = outPoint;
    if(!silent) emit changed();
}

void MediaInfo::setOutPoint(QString outPoint, bool silent)
{
    setOutPoint( MediaUtils::timecodeToDuration(outPoint), silent );
}

double MediaInfo::inPoint() const
{
    return _inPoint;
}

void MediaInfo::setInPoint(double inPoint, bool silent)
{
    _inPoint = inPoint;
    if(!silent) emit changed();
}

void MediaInfo::setInPoint(QString inPoint, bool silent)
{
    setInPoint( MediaUtils::timecodeToDuration(inPoint), silent );
}

QStringList MediaInfo::emptyFrames() const
{
    return _emptyFrames;
}

QList<int> MediaInfo::missingFrames() const
{
    return _missingFrames;
}

QString MediaInfo::info() const
{
    return _info;
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

void MediaInfo::setAep(bool isAep, bool silent )
{
    _isAep = isAep;
    addVideoStream(new VideoInfo(this));
    AudioInfo *s = new AudioInfo(this);
    s->setChannels("stereo");
    addAudioStream(s);
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
    if (isSequence() && _videoStreams.count() >= 1)
    {
        int numFrames = _frames.count();
        double frameRate = _videoStreams[0]->framerate();
        if (frameRate == 0) frameRate = 24.0;
        return numFrames / frameRate;
    }
    return _duration;
}

QString MediaInfo::fileName()
{
    return _fileName;
}

FFCodec *MediaInfo::defaultVideoCodec() const
{
    return _muxer->defaultVideoCodec();
}

FFCodec *MediaInfo::defaultAudioCodec() const
{
    return _muxer->defaultAudioCodec();
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
    return _muxer->isSequence();
}

QStringList MediaInfo::extensions() const
{
    if (_muxer->extensions().count() == 0) return _extensions;
    return _muxer->extensions();
}

FFMuxer *MediaInfo::muxer() const
{
    return _muxer;
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
    if (vc->name() == "") return false;

    foreach (FFPixFormat *pf, vc->pixFormats())
    {
        if (pf->hasAlpha()) return true;
    }

    return false;
}

void MediaInfo::setAlpha(bool alpha, bool silent)
{
    FFCodec *vc = defaultVideoCodec();
    FFPixFormat *pf = vc->defaultPixFormat(alpha);

    foreach( VideoInfo *stream, _videoStreams)
    {
        bool ok = stream->setAlpha(alpha, silent);
        if (ok) continue;
        stream->setPixFormat( pf );
    }
}

FFColorProfile *MediaInfo::defaultColorProfile(int streamId)
{
    //Try to get from muxer
    QString profileName = _muxer->defaultColorProfile();
    //If not found, let's check the stream
    if (profileName == "" && streamId >=0 && streamId < _videoStreams.count())
    {
        VideoInfo *stream = _videoStreams[streamId];
        //Try to get from the codec
        FFCodec *c = stream->codec();
        if (c) profileName = c->defaultColorProfile();
        //Try to get from pixel format
        if (profileName =="")
        {
            FFPixFormat *pixFmt = pixFormat(streamId);
            if (pixFmt) profileName = pixFmt->defaultColorProfile();
        }
    }
    return FFmpeg::instance()->colorProfile( profileName );
}

FFPixFormat *MediaInfo::pixFormat(int streamId)
{
    FFPixFormat *pixFmt = nullptr;
    if (streamId >=0 && streamId < _videoStreams.count())
    {
        VideoInfo *stream = _videoStreams[streamId];
        pixFmt = stream->pixFormat();
        if (pixFmt->name() == "") pixFmt = stream->defaultPixFormat();
        if (pixFmt->name() == "") pixFmt = this->defaultPixFormat();
    }
    return pixFmt;
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
    return c->defaultPixFormat();
}

QStringList MediaInfo::frames() const
{
    return _frames;
}

void MediaInfo::loadSequence(bool silent)
{
    if (_fileName == "") return;
    if (_videoStreams.count() == 0) return;

    qDebug() << "Loading sequence from " + _fileName;

    _videoStreams[0]->setIntra(true, silent);
    _videoStreams[0]->setSequence(true);

    //base file info
    QFileInfo baseFileInfo(_fileName);
    QString extension = baseFileInfo.suffix();
    QString baseName = baseFileInfo.completeBaseName();
    QString dirPath = baseFileInfo.path();

    QRegularExpression regExDigits("{(#+)}");

    //find digits in the name (if this is not an output, and file exist)
    if (!_outputMedia)
    {
        _frames.clear();
        _videoStreams[0]->setStartNumber(0);

        QRegularExpression reDigits("(\\d+)");
        QRegularExpressionMatchIterator reDigitsMatch = reDigits.globalMatch(baseName);

        //get all files of same type in folder
        QDir containingDir(dirPath);
        QFileInfoList files = containingDir.entryInfoList(QStringList("*." + extension),QDir::Files);

        //check for each block of digits if we find a sequence
        QString error = "";
        QStringList missingFrames;
        QStringList emptyFrames;
        bool incorrect = false;

        qDebug() << "Scanning file name.";

        while(reDigitsMatch.hasNext())
        {
            incorrect = false;

            QStringList tempFrames;
            QRegularExpressionMatch match = reDigitsMatch.next();
            QString digits = match.captured(0);

            //Get frames and check if this is a sequence
            //list all files in the sequence matching the pattern, and compute size
            QString left = baseName.left(match.capturedStart(0));
            QString right = baseName.right(baseName.count() - match.capturedEnd(0));
            QString pattern = left + "(\\d+)" + right;
            QRegularExpression re(pattern);
            _videoStreams[0]->setStartNumber( 999999999 );
            _endNumber = 0;
            _emptyFrames.clear();
            _missingFrames.clear();
            _size = 0;
            int numDigits = 0;
            foreach(QFileInfo f,files)
            {
                QRegularExpressionMatch reMatch = re.match(f.completeBaseName());
                if (reMatch.hasMatch())
                {
                    //check num digits
                    int testNumDigits = reMatch.captured(1).count();
                    if (numDigits != 0 && numDigits != testNumDigits)
                    {
                        incorrect = true;
                        qDebug() << "Wrong naming, missing leading zeroes";
                        if (error == "") error = "Incorrect sequence file names.\nNumbers must have leading zeroes (\"8, 9, 10, 11\" has to be \"08, 09, 10, 11\").\n";
                    }
                    numDigits = testNumDigits;
                    //get start frame
                    int currentNumber = reMatch.captured(1).toInt();
                    if (currentNumber < _videoStreams[0]->startNumber()) _videoStreams[0]->setStartNumber( currentNumber );
                    if (currentNumber > _endNumber) _endNumber = currentNumber;
                    tempFrames << f.filePath();
                    _size += f.size();
                    if (f.size() < 10) _emptyFrames << f.filePath();
                }
            }

            //Check if numbering is ok
            int numFrames = _endNumber - _videoStreams[0]->startNumber() + 1;
            if (_endNumber == _videoStreams[0]->startNumber())
            {
                qDebug() << "Wrong digits block.";
                error = "This does not look like a sequence, we did not find the frame number.";
                incorrect = true;
            }

            if (numFrames != tempFrames.count() && !incorrect)
            {
                incorrect = true;
                error = "Some frames are missing.\n";
                tempFrames.sort();
                int num = _videoStreams[0]->startNumber() - 1;
                //look for missing frames
                foreach(QFileInfo f, tempFrames)
                {
                    QRegularExpressionMatch test = reDigits.match(f.completeBaseName());
                    int testNum = test.captured(0).toInt();
                    num++;
                    while (num < testNum)
                    {
                        _missingFrames << num;
                        num++;
                    }
                    num = testNum;
                }
            }

            //we've found the digits block
            if (!incorrect)
            {
                qDebug() << "Found " + QString::number(tempFrames.count()) + " frames.";
                error = "";
                //list all files in the sequence matching the pattern, and compute size
                _frames = tempFrames;
                _bitrate = ( _size * 8 ) / ( _frames.count()/24.0 );
                if (_videoStreams[0]->startNumber() == 999999999) _videoStreams[0]->setStartNumber(0);
                //update filename with dume convention
                QString digitsBlock = "";
                while(digitsBlock.count() < digits.count())
                {
                    digitsBlock += "#";
                }
                _fileName = dirPath + "/" + left + "{" + digitsBlock + "}" + right + "." + extension;
                qDebug() << "Naming scheme: " + _fileName;
                break;
            }
        }

        qDebug() << "Finished scanning file name.";

        if (incorrect)
        {
            _info += error;
            qDebug() << "Not a sequence/digits not found.";
            return;
        }

        qDebug() << "Found digits, generating ffmpeg name.";
    }
    else
    {
        if (!_fileName.contains(regExDigits))
        {
            _fileName = baseName + "_{#####}." + extension;
        }
    }

    //generates the sequence name used by ffmpeg
    //detects all {###}

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

    qDebug() << "Sequence found!";
}

int MediaInfo::loop() const
{
    return _loop;
}
