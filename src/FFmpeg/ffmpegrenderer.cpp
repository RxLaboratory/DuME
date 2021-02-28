#include "ffmpegrenderer.h"

#include <QtDebug>

// Singleton, instance is nullptr by default until instance() is called for the first time
FFmpegRenderer *FFmpegRenderer::_instance = nullptr;

FFmpegRenderer *FFmpegRenderer::instance()
{
    if (!_instance) _instance = new FFmpegRenderer();
    return _instance;
}

FFmpegRenderer::FFmpegRenderer(QObject *parent) : AbstractRenderer(parent)
{
    _ffmpeg = FFmpeg::instance();

    // Let's work in BT.2020_12 by default, which is fully compatible with ffmpeg and has a wide gammut
    //_workingColorProfile = _ffmpeg->colorProfile( "bt2020_12" );
    // Or in input space
    _workingColorProfile = _ffmpeg->colorProfile( "input" );

    initJob();
}

bool FFmpegRenderer::launchJob()
{
    qDebug() << "Launching FFMpeg Job";
    setStatus( MediaUtils::Launching );

    // init job
    initJob();

    // prepare inputs
    foreach( MediaInfo *input, _job->getInputMedias() ) setupInput(input);

    // setup outputs
    foreach(MediaInfo *output, _job->getOutputMedias()) setupOutput(output);

    emit newLog("Beginning new encoding\nUsing FFmpeg input:\n" + _inputArgs.join(" | ") + "\nUsing FFmpeg output:\n" + _outputArgs.join(" | "));

    //launch
    this->setOutputFileName( _job->getOutputMedias().at(0)->fileName() );

    if (_jobFramerate != 0.0)
    {
        this->setNumFrames( _jobDuration * _jobFramerate / _speedMultiplicator );
        this->setFrameRate( _jobFramerate );
    }

    this->start( _inputArgs + _outputArgs );
    return true;
}

void FFmpegRenderer::initJob()
{
    _inputArgs.clear();
    _outputArgs.clear();

    _jobFramerate = 0.0;
    _jobDuration = 0.0;

    _inputPrimaries = nullptr;
    _inputTrc = nullptr;

    _inputArgs << "-loglevel" << "error" << "-stats" << "-y";
}

void FFmpegRenderer::setupInput(MediaInfo *inputMedia)
{
    // add custom options
    _inputArgs += getFFmpegCustomOptions( inputMedia );

    // add video settings
    if (inputMedia->hasVideo())
    {
        // Getting settings from the first video stream
        VideoInfo *videoStream = inputMedia->videoStreams().at(0);

        // Update job framerate
        if (videoStream->framerate() != 0.0) _jobFramerate = videoStream->framerate();
        else _jobFramerate = 24;

        // Get Sequence settings
        _inputArgs += getInputSequenceSettings( videoStream );

        // Get Color metadata
        _inputArgs += getColorMetadata( videoStream, inputMedia->defaultColorProfile(), true );
    }

    // Update job duration
    double testDuration = getMediaDuration( inputMedia );
    if (testDuration > _jobDuration) _jobDuration = testDuration;

    // Time Range
    _inputArgs += getTimeRange( inputMedia );

    // Filename
    _inputArgs << "-i" << getFileName( inputMedia );
}

QStringList FFmpegRenderer::getFFmpegCustomOptions(MediaInfo *media)
{
    QStringList customArgs;

    foreach(QStringList option,media->ffmpegOptions())
    {
        QString opt = option[0];

        //Ignore video filters, they're added with the other filters
        if (opt == "-filter:v" || opt == "-vf") continue;

        customArgs << opt;
        if (option.count() > 1) if (option[1] != "") customArgs << option[1];
    }

    return customArgs;
}

QStringList FFmpegRenderer::getInputSequenceSettings(VideoInfo *sequence)
{
    QStringList sequenceSettings;
    if ( !sequence->isSequence() ) return sequenceSettings;

    // First frame number
    sequenceSettings << "-start_number" << QString::number(sequence->startNumber());
    // Framerate
    sequenceSettings << "-framerate" << QString::number( _jobFramerate );

    return sequenceSettings;
}

QStringList FFmpegRenderer::getColorMetadata(VideoInfo *videoStream, FFColorProfile *defaultProfile, bool isInput)
{
    QStringList colorArgs;

    // Get the default Color profile
    QString profileName = defaultProfile->name();

    // TRC
    FFColorItem *trc = videoStream->colorTRC();
    if (trc->metadataName() == "" && profileName != "") trc = defaultProfile->trc();
    if (trc->metadataName() != "") colorArgs << "-color_trc" << trc->metadataName();
    if (isInput) _inputTrc = trc;

    if (videoStream->colorTRC()->metadataName() != "") colorArgs << "-color_trc" << videoStream->colorTRC()->metadataName();
    else if ( profileName != "" ) colorArgs << "-color_trc" << defaultProfile->trc()->metadataName();

    // RANGE
    if (videoStream->colorRange()->metadataName() != "") colorArgs << "-color_range" << videoStream->colorRange()->metadataName();
    else if ( profileName != "" ) colorArgs << "-color_range" << defaultProfile->range()->metadataName();

    // PRIMARIES
    FFColorItem *primaries = videoStream->colorPrimaries();
    if (primaries->metadataName() == "" && profileName != "") primaries = defaultProfile->primaries();
    if (primaries->metadataName() != "") colorArgs << "-color_primaries" << primaries->metadataName();
    if (isInput) _inputPrimaries = primaries;

    // MATRIX
    if (videoStream->colorSpace()->metadataName() != "") colorArgs << "-colorspace" << videoStream->colorSpace()->metadataName();
    else if ( profileName != "" ) colorArgs << "-colorspace" << defaultProfile->space()->metadataName();

    return colorArgs;
}

double FFmpegRenderer::getMediaDuration(MediaInfo *media)
{
    if (media->inPoint() != 0.0 || media->outPoint() != 0.0) return media->outPoint() - media->inPoint();

    if (_jobFramerate == 0.0) _jobFramerate = 24.0;
    if (media->isSequence()) return media->frames().count() / _jobFramerate;

    return media->duration();
}

QStringList FFmpegRenderer::getTimeRange(MediaInfo *media)
{
    QStringList timeRangeArgs;
    if (media->inPoint() != 0.0) timeRangeArgs << "-ss" << QString::number( media->inPoint() );
    if (media->outPoint() != 0.0) timeRangeArgs << "-to" << QString::number( media->outPoint() );
    return timeRangeArgs;
}

QString FFmpegRenderer::getFileName(MediaInfo *media)
{
    QString filename;
    if ( media->isSequence() ) filename = media->ffmpegSequenceName();
    else filename = media->fileName();
    return QDir::toNativeSeparators( filename );
}

void FFmpegRenderer::setupOutput(MediaInfo *outputMedia)
{
    //maps
    _outputArgs += getMaps( outputMedia );

    //muxer
    _outputArgs += getMuxer( outputMedia );

    //custom options
    _outputArgs += getFFmpegCustomOptions( outputMedia );

    //video
    if (outputMedia->hasVideo())
    {
        // There's only a single video stream in outputs in DuME for now.
        VideoInfo *videoStream = outputMedia->videoStreams().at(0);

        // Codec
        FFCodec *codec = getFFCodec( videoStream, outputMedia->defaultVideoCodec() );
        _outputArgs += getCodec( videoStream );

        if (codec->name() != "copy")
        {
            // We need the pixel format later
            FFPixFormat *pixFormat = getPixelFormat( outputMedia, videoStream, codec);

            // Bitrate
            _outputArgs += getBitRate( videoStream, codec);
            // Framerate
            _outputArgs += getFramerate( videoStream );
            // Loop
            _outputArgs += getLoop( outputMedia, codec );
            // Codec Settings
            _outputArgs += getCodecSettings( videoStream, codec, pixFormat );
            // Sequence settings
            _outputArgs += getOutputSequenceSettings(videoStream);
            // Pixel format
            _outputArgs += getPixelFormatSettings( videoStream, pixFormat);
            // Color Metadata
            if ( videoStream->colorConversionMode() != MediaUtils::Convert) _outputArgs += getColorMetadata( videoStream, outputMedia->defaultColorProfile() );
            // Filters
            _outputArgs += getFilters( outputMedia, videoStream );
        }
    }
    else _outputArgs += "-vn";


    //audio
    if (outputMedia->hasAudio())
    {
        // There's only a single audio stream in outputs in DuME for now.
        AudioInfo *audioStream = outputMedia->audioStreams().at(0);

        //codec
        FFCodec *codec = getFFCodec( audioStream, outputMedia->defaultAudioCodec() );
        _outputArgs += getCodec( audioStream );

        if (codec->name() != "copy")
        {
            //bitrate
            _outputArgs += getBitRate( audioStream );

            //sampling
            _outputArgs += getSampling( audioStream );

            //sample format
            _outputArgs += getSampleFormat( audioStream );
        }
    }
    else _outputArgs += "-an";

    //file
    QString outputPath = getFileName( outputMedia );

    _outputArgs << outputPath;
}

QStringList FFmpegRenderer::getMaps(MediaInfo *media)
{
    QStringList maps;

    foreach (StreamReference map, media->maps())
    {
        int mediaId = map.mediaId();
        int streamId = map.streamId();
        if (mediaId >= 0 && streamId >= 0) maps << "-map" << QString::number( mediaId ) + ":" + QString::number( streamId );
    }

    return maps;
}

QStringList FFmpegRenderer::getMuxer(MediaInfo *media)
{
    QStringList muxerArgs;

    FFMuxer *m = media->muxer();
    if (!m) return muxerArgs;

    QString muxer = m->name();
    if (m->isSequence()) muxer = "image2";

    if (muxer != "") muxerArgs << "-f" << muxer;

    return muxerArgs;
}

FFCodec *FFmpegRenderer::getFFCodec(VideoInfo *stream, FFCodec *defaultCodec)
{
    FFCodec *vc = stream->codec();
    if (!vc) return defaultCodec;
    if (vc->name() != "") return vc;
    return defaultCodec;
}

FFCodec *FFmpegRenderer::getFFCodec(AudioInfo *stream, FFCodec *defaultCodec)
{
    FFCodec *ac = stream->codec();
    if (!ac) return defaultCodec;
    if (ac->name() != "") return ac;
    return defaultCodec;
}

QStringList FFmpegRenderer::getCodec(VideoInfo *stream)
{
    QStringList codecArgs;

    FFCodec *vc = stream->codec();
    if (!vc) return codecArgs;
    if (vc->name() != "") codecArgs << "-c:v" << vc->name();
    return codecArgs;
}

QStringList FFmpegRenderer::getCodec(AudioInfo *stream)
{
    QStringList codecArgs;

    FFCodec *ac = stream->codec();
    if (!ac) return codecArgs;
    if (ac->name() != "") codecArgs << "-c:a" << ac->name();
    return codecArgs;
}

QStringList FFmpegRenderer::getBitRate(VideoInfo *stream, FFCodec *codec)
{
    QStringList bitrateArgs;
    //bitrate
    int bitrate = int( stream->bitrate() );
    if (bitrate != 0) bitrateArgs << "-b:v" << QString::number(bitrate);

    //bitrate type
    if (codec->useBitrateType() && stream->bitrateType() == MediaUtils::BitrateType::CBR)
    {
        bitrateArgs << "-minrate" << QString::number(bitrate);
        bitrateArgs << "-maxrate" << QString::number(bitrate);
        bitrateArgs << "-bufsize" << QString::number(bitrate*2);
    }

    return bitrateArgs;
}

QStringList FFmpegRenderer::getBitRate(AudioInfo *stream)
{
    QStringList bitrateArgs;
    int bitrate = int( stream->bitrate() );
    if (bitrate != 0)
    {
        bitrateArgs << "-b:a" << QString::number(stream->bitrate());
    }
    return bitrateArgs;
}

QStringList FFmpegRenderer::getSampling(AudioInfo *stream)
{
    QStringList samplingArgs;
    int sampling = stream->samplingRate();
    if (sampling != 0)
    {
        samplingArgs << "-ar" << QString::number(sampling);
    }
    return samplingArgs;
}

QStringList FFmpegRenderer::getSampleFormat(AudioInfo *stream)
{
    QStringList sampleArgs;
    QString sampleFormat = stream->sampleFormat()->name();
    if (sampleFormat != "")
    {
        sampleArgs << "-sample_fmt" << sampleFormat;
    }
    return sampleArgs;
}

QStringList FFmpegRenderer::getFramerate(VideoInfo *stream)
{
    QStringList framerateArgs;

    if (stream->framerate() != 0.0)
    {
        framerateArgs << "-r" << QString::number(stream->framerate());
        _jobFramerate = stream->framerate();
    }

    return framerateArgs;
}

QStringList FFmpegRenderer::getLoop(MediaInfo *media, FFCodec *codec)
{
    QStringList loopArgs;
    if (codec->name() == "gif")
    {
        int loop = media->loop();
        loopArgs << "-loop" << QString::number(loop);
    }
    return loopArgs;
}

QStringList FFmpegRenderer::getCodecSettings(VideoInfo *stream, FFCodec *codec, FFPixFormat *pixFormat)
{
    QStringList codecSettings;

    if (stream->level() != "") codecSettings << "-level" << stream->level();

    //quality
    int quality = stream->quality();
    if ( quality >= 0 && codec->name() != "" && codec->qualityParam() != "") codecSettings << codec->qualityParam() << codec->qualityValue(quality);

    //encoding speed
    int speed = stream->encodingSpeed();
    if (codec->useSpeed() && speed >= 0) codecSettings << codec->speedParam() << codec->speedValue(speed);

    //fine tuning
    if (codec->useTuning() && stream->tuning()->name() != "")
    {
        codecSettings << "-tune" << stream->tuning()->name();
        if (stream->tuning()->name() == "zerolatency") codecSettings << "-movflags" << "+faststart";
    }

    //profile
    if (stream->profile()->name() != "" && codec->useProfile())
    {
        QString profile = stream->profile()->name();
        // Adjust h264 main profile
        if ( (codec->name() == "h264" || codec->name() == "libx264") && profile == "high")
        {
            if (stream->pixFormat()->name() != "")
            {
                if (pixFormat->yuvComponentsDistribution() == "444") profile = "high444";
                else if (pixFormat->yuvComponentsDistribution() == "422") profile = "high422";
                else if (pixFormat->bitsPerPixel() > 12) profile = "high10"; // 12 bpp at 420 is 8bpc
            }
        }
        codecSettings << "-profile:v" << profile;
    }

    // h265 options
    if (codec->name() == "hevc" || codec->name() == "h265" || codec->name() == "libx265")
    {
        QStringList x265options;
        if (stream->intra())
        {
            float ffmepgVersion = FFmpeg::instance()->version().left(3).toFloat() ;
            if (ffmepgVersion >= 4.3)
            {
                codecSettings << "-g" << "1";
            }
            else
            {
                x265options << "keyint=1";
            }
        }
        if (stream->lossless() && stream->quality() >= 0)
        {
            x265options << "lossless=1";
        }
        if (x265options.count() > 0)
        {
            codecSettings << "-x265-params" << "\"" + x265options.join(":") + "\"";
        }
    }

    // h264 options
    if (codec->name() == "h264" || codec->name() == "libx264")
    {
        if (stream->intra())
        {
            float ffmepgVersion = FFmpeg::instance()->version().left(3).toFloat() ;
            if (ffmepgVersion >= 4.3) codecSettings << "-g" << "1";
            else codecSettings << "-intra";
        }
        //b-pyramids
        //set as none to h264: not really useful (only on very static footage), but has compatibility issues
        codecSettings << "-x264opts" << "b_pyramid=0";
    }

    // prores options
    if (codec->name().indexOf("prores") >= 0)
    {
        //set to ap10 with prores for improved compatibility
        codecSettings << "-vendor" << "ap10";
    }

    return codecSettings;
}

QStringList FFmpegRenderer::getOutputSequenceSettings(VideoInfo *stream)
{
    QStringList sequenceSettings;
    if (!stream->isSequence()) return sequenceSettings;

    int startNumber = stream->startNumber();
    sequenceSettings << "-start_number" << QString::number(startNumber);

    return sequenceSettings;
}

FFPixFormat *FFmpegRenderer::getPixelFormat(MediaInfo *media, VideoInfo *stream, FFCodec *codec)
{
    FFPixFormat *pixFormat = stream->pixFormat();

    //set default for h264 to yuv420 (ffmpeg generates 444 by default which is not standard)
    if (pixFormat->name() == "" && (codec->name() == "h264" || codec->name() == "libx264") ) pixFormat = _ffmpeg->pixFormat("yuv420p");

    QString muxerName = "";
    FFMuxer *m = media->muxer();
    if (m) muxerName = m->name();

    if (pixFormat->name() == "" && muxerName == "mp4") pixFormat = _ffmpeg->pixFormat("yuv420p");

    return pixFormat;
}

QStringList FFmpegRenderer::getPixelFormatSettings(VideoInfo *stream, FFPixFormat *pixFormat)
{
    QStringList pixelArgs;

    if (pixFormat->name() != "") pixelArgs << "-pix_fmt" << pixFormat->name();

    // video codecs with alpha need to set -auto-alt-ref to 0
    if (pixFormat->hasAlpha() && !stream->isSequence()) pixelArgs << "-auto-alt-ref" << "0";

    return pixelArgs;
}

QStringList FFmpegRenderer::getFilters(MediaInfo *media, VideoInfo *stream)
{
    QStringList filterChain;

    //convert colors to working space
    filterChain += inputColorConversionFilters();

    //apply filters

    //unpremultiply
    filterChain << unpremultiplyFilter( stream );
    //deinterlace
    filterChain << deinterlaceFilter( stream );
    //motion
    filterChain += motionFilters( stream );
    //crop
    filterChain << cropFilter( stream );
    //Collect custom
    filterChain += customVideoFilters( media );
    //1D / 3D LUT (before color management)
    if (!stream->applyLutOnOutputSpace()) filterChain += applyLutFilters( stream, _workingColorProfile );
    //resize
    filterChain += resizeFilters( stream );

    // COLOR MANAGEMENT
    FFColorProfile *outputProfile = new FFColorProfile("temp", "temp", stream->colorPrimaries(), stream->colorTRC(), stream->colorSpace(), stream->colorRange());
    filterChain += colorConversionFilters( _workingColorProfile, outputProfile );

    //1D / 3D LUT (on output)
    if (stream->applyLutOnOutputSpace()) filterChain += applyLutFilters( stream, outputProfile );

    outputProfile->deleteLater();

    //compile filters
    filterChain.removeAll(QString(""));

    QStringList filters;
    if (filterChain.count() > 0) filters << "-vf" << filterChain.join(",");
    return filters;
}

QString FFmpegRenderer::unpremultiplyFilter(VideoInfo *stream)
{
    bool unpremultiply = !stream->premultipliedAlpha();
    if (unpremultiply) return "unpremultiply=inplace=1";
    return "";
}

QString FFmpegRenderer::deinterlaceFilter(VideoInfo *stream)
{
    if (!stream->deinterlace()) return "";

    QString deinterlaceOption = "yadif=parity=";
    if (stream->deinterlaceParity() == MediaUtils::TopFieldFirst) deinterlaceOption += "0";
    else if (stream->deinterlaceParity() == MediaUtils::BottomFieldFirst) deinterlaceOption += "1";
    else deinterlaceOption += "-1";
    return deinterlaceOption;
}

QStringList FFmpegRenderer::motionFilters(VideoInfo *stream)
{
    QStringList filters;

    //speed
    if (stream->speed() != 1.0) filters << "setpts=" + QString::number(1/stream->speed()) + "*PTS";
    _speedMultiplicator = stream->speed();

    //motion interpolation
    if (stream->speedInterpolationMode() != MediaUtils::NoMotionInterpolation)
    {
        QString speedFilter = "minterpolate='";
        if (stream->speedInterpolationMode() == MediaUtils::DuplicateFrames) speedFilter += "mi_mode=dup";
        else if (stream->speedInterpolationMode() == MediaUtils::BlendFrames) speedFilter += "mi_mode=blend";
        else
        {
            speedFilter += "mi_mode=mci";
            if (stream->speedInterpolationMode() == MediaUtils::MCIO) speedFilter += ":mc_mode=obmc";
            else if (stream->speedInterpolationMode() == MediaUtils::MCIAO) speedFilter += ":mc_mode=aobmc";
            if (stream->speedEstimationMode()->name() != "") speedFilter += ":me_mode=" + stream->speedEstimationMode()->name();
            if (stream->speedAlgorithm()->name() != "") speedFilter += ":me=" + stream->speedAlgorithm()->name();
        }
        if (!stream->sceneDetection()) speedFilter += ":scd=none";
        else speedFilter += ":scd=fdiff";
        double framerate = stream->framerate();
        //get framerate from input
        if (framerate == 0.0) framerate = _jobFramerate;
        if (framerate > 0.0) speedFilter += ":fps=" + QString::number(framerate);
        speedFilter += "'";
        filters << speedFilter;
    }

    return filters;
}

QString FFmpegRenderer::cropFilter(VideoInfo *stream)
{
    if (!stream->cropUseSize() && ( stream->topCrop() != 0 || stream->bottomCrop() != 0 || stream->leftCrop() != 0 || stream->rightCrop() != 0))
    {
        QString left = QString::number(stream->leftCrop());
        QString right = QString::number(stream->rightCrop());
        QString top = QString::number(stream->topCrop());
        QString bottom = QString::number(stream->bottomCrop());
        return "crop=in_w-" + left + "-" + right + ":in_h-" + top + "-" + bottom + ":" + left + ":" + top;
    }

    if (stream->cropUseSize() && ( stream->cropHeight() != 0 || stream->cropWidth() != 0) )
    {
        int wi = stream->cropWidth();
        int hi = stream->cropHeight();
        QString w = QString::number( wi );
        if (wi == 0) w = "in_w";
        QString h = QString::number( hi );
        if (hi == 0) h = "in_h";
        return "crop=" + w + ":" + h;
    }

    return "";
}

QStringList FFmpegRenderer::customVideoFilters(MediaInfo *media)
{
    QStringList filters;
    foreach(QStringList option, media->ffmpegOptions())
    {
        QString opt = option[0];
        //Get video filters
        if (opt != "-filter:v" && opt != "-vf") continue;
        if (option.count() > 1)
        {
            if (option[1] != "") filters << option[1];
        }
    }
    return filters;
}

QStringList FFmpegRenderer::colorConversionFilters(FFColorItem *outputTrc, FFColorItem *outputPrimaries, FFColorItem *outputMatrix, FFColorItem *outputRange, FFColorItem *inputTrc, FFColorItem *inputPrimaries, FFColorItem *inputMatrix, FFColorItem *inputRange)
{
    QStringList filters;

    // Color conversion filters
    QStringList zScaleArgs;
    QStringList colorspaceArgs;
    QStringList lutrgbArgs;
    QStringList outputLut3dArgs;

    // Now we can convert
    // TRC
    if (outputTrc)
    {
        if (outputTrc->isOutput())
        {
            // Select the most appropriate filter
            FFColorItem::ScaleFilter filter = outputTrc->selectOutputFilter( inputTrc );

            if (filter == FFColorItem::ZScale)
            {
                if ( inputTrc ) if ( inputTrc->inputZScaleName() != "" ) zScaleArgs << "transferin=" + inputTrc->inputZScaleName();
                zScaleArgs << "transfer=" + outputTrc->outputZScaleName();
            }
            else if (filter == FFColorItem::Colorspace)
            {
                if ( inputTrc ) if ( inputTrc->inputCSScaleName() != "" ) colorspaceArgs << "itrc=" + inputTrc->inputCSScaleName();
                colorspaceArgs << "trc=" + outputTrc->outputZScaleName();
            }
            else if (filter == FFColorItem::Gamma)
            {
                //linearize first
                if ( inputTrc ) if ( inputTrc->inputZScaleName() != "" ) zScaleArgs << "transferin=" + inputTrc->inputZScaleName();
                zScaleArgs << "transfer=linear";
                //apply gamma
                lutrgbArgs << "r=gammaval(" + outputTrc->outputGScaleName() + ")";
                lutrgbArgs << "g=gammaval(" + outputTrc->outputGScaleName() + ")";
                lutrgbArgs << "b=gammaval(" + outputTrc->outputGScaleName() + ")";
            }
        }
    }

    // PRIMARIES
    if (outputPrimaries)
    {
        if (outputPrimaries->isOutput())
        {
            // Select the most appropriate filter
            FFColorItem::ScaleFilter filter = outputPrimaries->selectOutputFilter( inputPrimaries );

            if (filter == FFColorItem::ZScale)
            {
                if ( inputPrimaries ) if ( inputPrimaries->inputZScaleName() != "" ) zScaleArgs << "primariesin=" + inputPrimaries->inputZScaleName();
                zScaleArgs << "primaries=" + outputPrimaries->outputZScaleName();
            }
            else if (filter == FFColorItem::Colorspace)
            {
                if ( inputPrimaries ) if ( inputPrimaries->inputCSScaleName() != "" ) colorspaceArgs << "iprimaries=" + inputPrimaries->inputCSScaleName();
                colorspaceArgs << "primaries=" + outputPrimaries->outputZScaleName();
            }
            else if (filter == FFColorItem::LUT)
            {
                //get and extract lut
                FFLut *lut = FFmpeg::instance()->lut( outputPrimaries->outputLScaleName() );
                //convert to lut input space
                FFColorProfile *lutInputProfile = _ffmpeg->colorProfile( lut->inputProfile() );
                filters += colorConversionFilters( nullptr, lutInputProfile->primaries(), nullptr, nullptr, nullptr, inputPrimaries );
                outputLut3dArgs << lut->extract().replace("\\","/");
            }
        }
    }

    // RANGE
    if (outputRange)
    {
        if (inputRange) zScaleArgs << "rangein=" + inputRange->inputZScaleName();
        zScaleArgs << "range=" + outputRange->outputZScaleName();
    }

    //MATRIX
    if (outputMatrix)
    {
        if (outputMatrix->isOutput())
        {
            // Select the most appropriate filter
            FFColorItem::ScaleFilter filter = outputMatrix->selectOutputFilter( inputMatrix );

            if (filter == FFColorItem::ZScale)
            {
                if (inputMatrix) if (inputMatrix->inputZScaleName() != "") zScaleArgs << "matrixin=" + inputMatrix->inputZScaleName();
                zScaleArgs << "matrix=" + outputMatrix->outputZScaleName();
            }
            else if (filter == FFColorItem::Colorspace)
            {
                if (inputMatrix) if (inputMatrix->inputCSScaleName() != "") colorspaceArgs << "ispace=" + inputMatrix->inputCSScaleName();
                colorspaceArgs << "space=" + outputMatrix->outputCSScaleName();
            }
        }
    }

    if (zScaleArgs.count() > 0)
    {
        zScaleArgs << "dither=ordered";
        filters << generateFilter("zscale", zScaleArgs);
    }
    if (colorspaceArgs.count() > 0)
    {
        filters << generateFilter("colorspace", colorspaceArgs);
    }
    if (lutrgbArgs.count() > 0)
    {
        filters << generateFilter("lutrgb", lutrgbArgs);
    }
    if (outputLut3dArgs.count() > 0)
    {
        filters << generateFilter("lut3d", outputLut3dArgs);
    }

    return filters;
}

QStringList FFmpegRenderer::colorConversionFilters(FFColorProfile *from, FFColorItem *outputTrc, FFColorItem *outputPrimaries, FFColorItem *outputMatrix, FFColorItem *outputRange)
{
    return colorConversionFilters(outputTrc, outputPrimaries, outputMatrix, outputRange, from->trc(), from->primaries(), from->space(), from->range());
}

QStringList FFmpegRenderer::colorConversionFilters(FFColorProfile *outputProfile)
{
    if (outputProfile->name() == "") return QStringList();
    return colorConversionFilters(outputProfile->trc(), outputProfile->primaries(), outputProfile->space(), outputProfile->range());
}

QStringList FFmpegRenderer::colorConversionFilters(FFColorProfile *from, FFColorProfile *to)
{
    if (to->name() == "") return QStringList();
    return colorConversionFilters(to->trc(), to->primaries(), to->space(), to->range(), from->trc(), from->primaries(), from->space(), from->range());
}

QStringList FFmpegRenderer::inputColorConversionFilters()
{
    QStringList filters;

    if ( _workingColorProfile->name() == "") return filters;

    // Convert Colors from input for TRCs and Primaries' not supported by ffmpeg

    // Convert TRC
    if (_inputTrc)
    {
        if (_inputTrc->inputGScaleName() != "" && _inputTrc->defaultScaleFilter() == FFColorItem::Gamma)
        {
            QStringList filterArgs;
            filterArgs << "r=gammaval(" + _inputTrc->inputGScaleName() + ")";
            filterArgs << "g=gammaval(" + _inputTrc->inputGScaleName() + ")";
            filterArgs << "b=gammaval(" + _inputTrc->inputGScaleName() + ")";
            filters << generateFilter("lutrgb", filterArgs);
        }
    }

    // Convert Primaries
    if (_inputPrimaries)
    {
        if (_inputPrimaries->inputLScaleName() != "" && _inputPrimaries->defaultScaleFilter() == FFColorItem::LUT)
        {
            //get and extract lut
            FFLut *lut = FFmpeg::instance()->lut( _inputPrimaries->inputLScaleName() );
            QStringList filterArgs;
            filterArgs << lut->extract().replace("\\","/");
            filters << generateFilter("lut3d", filterArgs);
        }
    }

    // Convert to working profile
    filters += colorConversionFilters( _workingColorProfile );

    return filters;
}

QStringList FFmpegRenderer::applyLutFilters(VideoInfo *stream, FFColorProfile *colorProfile)
{
    QStringList lutFilters;
    // Get lut
    FFLut *lut = stream->lut();
    if (lut)
    {
        if (lut->name() != "" && lut->name() != "custom")
        {
            FFColorProfile *lutInputProfile = _ffmpeg->colorProfile( lut->inputProfile() );
            // Convert to input lut space
            if ( colorProfile->name() != "" && lutInputProfile->name() != "" && lutInputProfile->name() != colorProfile->name())
            {
                if (lut->type() == FFLut::OneD)
                    lutFilters += colorConversionFilters( lutInputProfile->trc() , nullptr, nullptr, nullptr, colorProfile->trc() );
                else if (lut->type() == FFLut::ThreeD)
                    lutFilters += colorConversionFilters( lutInputProfile->trc(), lutInputProfile->primaries(), nullptr, nullptr, colorProfile->trc(), colorProfile->primaries() );
            }

            // Apply lut
            lutFilters << getLutFilter(lut);

            // Convert back to previous space
            FFColorProfile *lutOutputProfile = _ffmpeg->colorProfile( lut->outputProfile() );
            if ( colorProfile->name() != "" && lutOutputProfile->name() != "" && lutOutputProfile->name() != colorProfile->name())
            {
                if (lut->type() == FFLut::OneD)
                    lutFilters += colorConversionFilters( colorProfile->trc() , nullptr, nullptr, nullptr, lutOutputProfile->trc() );
                else if (lut->type() == FFLut::ThreeD)
                    lutFilters += colorConversionFilters( colorProfile->trc(), colorProfile->primaries(), nullptr, nullptr, lutOutputProfile->trc(), lutOutputProfile->primaries() );
            }
        }
    }

    return lutFilters;
}

QStringList FFmpegRenderer::resizeFilters(VideoInfo *stream)
{
    QStringList sizeFilters;
    int width = stream->width();
    int height = stream->height();
    //fix odd sizes
    if (width % 2 != 0)
    {
        width--;
        emit newLog("Adjusting width for better compatibility. New width: " + QString::number(width));
    }
    if (height % 2 != 0)
    {
        height--;
        emit newLog("Adjusting height for better compatibility. New height: " + QString::number(height));
    }
    if (width != 0 || height != 0)
    {
       QString w = QString::number(width);
       QString h = QString::number(height);
       if (width == 0) w = "in_w";
       if (height == 0) h = "in_h";
       QString resizeAlgo = "";
       if (stream->resizeAlgorithm()->name() != "") resizeAlgo = ":flags=" + stream->resizeAlgorithm()->name();

       if (stream->resizeMode() == MediaUtils::Stretch)
       {
           sizeFilters << "scale=" + w + ":" + h + resizeAlgo;
           //we need to set the pixel aspect ratio back to 1:1 to force ffmpeg to stretch
           sizeFilters << "setsar=1:1";
       }
       else if (stream->resizeMode() == MediaUtils::Crop)
       {
           //first resize but keeping ratio (increase)
           sizeFilters << "scale=w=" + w + ":h=" + h + ":force_original_aspect_ratio=increase" + resizeAlgo;
           //then crop what's to large
           sizeFilters << "crop=" + w + ":" + h;
       }
       else if (stream->resizeMode() == MediaUtils::Letterbox)
       {
           //first resize but keeping ratio (decrease)
           sizeFilters << "scale=w=" + w + ":h=" + h + ":force_original_aspect_ratio=decrease" + resizeAlgo;
           //then pad with black bars
           sizeFilters << "pad=" + w + ":" + h + ":(ow-iw)/2:(oh-ih)/2";
       }
   }

    return sizeFilters;
}

bool FFmpegRenderer::colorManagement()
{
    qDebug() << "Color management";

    if (!_job) return false;

    MediaInfo *o = _job->getOutputMedias().at(0);
    if (!o->hasVideo()) return false;

    VideoInfo *ov = o->videoStreams().at(0);

    // If copy stream, nothing to convert
    if (ov->isCopy()) return false;

    qDebug() << "Check if no conversion param explicitly set";

    // If color profile is just embed, do not color convert anything
    if (ov->colorConversionMode() == MediaUtils::Embed) return false;

    qDebug() << "Check if the user explicitly wants to convert";

    // If any of the color components is set, we need to convert
    if (ov->colorPrimaries()->name() != "") return true;
    if (ov->colorRange()->name() != "") return true;
    if (ov->colorSpace()->name() != "") return true;
    if (ov->colorTRC()->name() != "") return true;

    // If the color profile of the output is different from the input, we do need to convert anyway.

    qDebug() << "Get the default output Color Profile";

    // Get the default Color profile
    FFColorProfile *outputProfile = o->defaultColorProfile();
    // We also need the pixel format, as we ultimately need to compare if space is changed
    FFPixFormat *outputPixFormat = o->pixFormat();

    qDebug() << outputProfile->prettyName();

    // Now let's check if any of the input has a different input color profile or color space
    foreach(MediaInfo *i, _job->getInputMedias())
    {
        if (!i->hasVideo()) continue;
        VideoInfo *iv = i->videoStreams()[0];

        qDebug() << "Check if the user explicitly wants to convert";

        // Check if any of the color components is set and different from the output
        if (iv->colorPrimaries()->name() != "" && iv->colorPrimaries()->name() != outputProfile->primaries()->name()) return true;
        if (iv->colorRange()->name() != "" && iv->colorRange()->name() != outputProfile->range()->name()) return true;
        if (iv->colorSpace()->name() != "" && iv->colorSpace()->name() != outputProfile->space()->name()) return true;
        if (iv->colorTRC()->name() != "" && iv->colorTRC()->name() != outputProfile->trc()->name()) return true;

        qDebug() << "Get the default input Color Profile";

        // Check from the pixel format
        FFColorProfile *inputProfile = i->defaultColorProfile();
        FFPixFormat *inputPixFormat = i->pixFormat();

        qDebug() << "Input: " + inputProfile->name();
        qDebug() << "Output: " + outputProfile->name();

        // If the profile is different, there is a color conversion
        if (inputProfile->name() != outputProfile->name()) return true;
        if (inputPixFormat->colorSpace() != outputPixFormat->colorSpace()) return true;
    }

    return false;
}

FFColorProfile *FFmpegRenderer::convertInputForLut()
{
    qDebug() << "Checking if a conversion is needed for input LUT";

    if (!_job) return nullptr;

    MediaInfo *o = _job->getOutputMedias().at(0);
    if (!o->hasVideo()) return nullptr;

    VideoInfo *ov = o->videoStreams().at(0);

    // If copy stream, nothing to convert
    if (ov->isCopy()) return nullptr;

    if (!ov->lut()) return nullptr;
    if (ov->lut()->name() == "") return nullptr;
    if (ov->lut()->name() == "custom") return nullptr;

    if (ov->applyLutOnOutputSpace()) return nullptr;

    FFColorProfile *lutInputProfile = FFmpeg::instance()->colorProfile( ov->lut()->inputProfile() );
    QString lutInputTrcName = lutInputProfile->trc()->name();
    QString lutInputPrimariesName = lutInputProfile->primaries()->name();

    // Now let's check if there's a need for conversion
    foreach(MediaInfo *i, _job->getInputMedias())
    {
        if (!i->hasVideo()) continue;
        VideoInfo *iv = i->videoStreams()[0];

        // Check if any of the color components is set and different from the lut input
        if (iv->colorPrimaries()->name() != "" && lutInputPrimariesName != "" && iv->colorPrimaries()->name() != lutInputPrimariesName && ov->lut()->type() == FFLut::ThreeD) return lutInputProfile;
        if (iv->colorTRC()->name() != "" && lutInputTrcName != "" && iv->colorTRC()->name() != lutInputTrcName ) return lutInputProfile;

        // Check from the pixel format
        FFColorProfile *inputProfile = i->defaultColorProfile();

        // If the profile is different, there is a color conversion
        if (inputProfile->name() != lutInputProfile->name()) return lutInputProfile;
    }

    return nullptr;
}

QString FFmpegRenderer::getLutFilter(FFLut *lut)
{
    QString filterName = "lut3d";
    QString lutName = lut->name();

    //check if it's 3D or 1D
    if (lutName.endsWith(".cube"))
    {
        QFile l(lutName);
        if (l.open(QIODevice::ReadOnly | QIODevice::Text))
        {
             QTextStream in(&l);
             QString line = in.readLine();
             while (!line.isNull())
             {
                 if (line.trimmed().toUpper().startsWith("LUT_1D"))
                 {
                     filterName = "lut1d";
                     break;
                 }
                 line = in.readLine();
             }
             l.close();
        }
    }

    return generateFilter(filterName, QStringList( lutName.replace("\\","/") ));
}

QString FFmpegRenderer::generateFilter(QString filterName, QStringList args)
{
    QString filter = filterName + "=";
    bool first = true;
    foreach(QString arg, args)
    {
        if (!first) filter += ":";
        filter += "'" + FFmpeg::escapeFilterOption( arg ) + "'";
    }
    return filter;
}

void FFmpegRenderer::readyRead(QString output)
{
    QRegularExpression reProgress = RegExUtils::getRegEx("ffmpeg progress");
    QRegularExpressionMatch match = reProgress.match(output);

    //if progress, update UI
    if (match.hasMatch())
    {
        QString frame = match.captured(1);
        QString size = match.captured(3);
        QString bitrate = match.captured(5);
        QString speed = match.captured(6);

         //size
        int sizeKB = size.toInt();

        //bitrate
        int bitrateKB = bitrate.toInt();

        //frame
        setCurrentFrame( frame.toInt(), sizeKB * 1024, bitrateKB * 1000, speed.toDouble() );

        emit progress();
    }
    //detect errors
    else if (!output.trimmed().startsWith("frame") && output.trimmed() != "")
    {
        //If we're rendering, all stuff which is not a progress is an error
        if (status() == MediaUtils::Encoding)
        {
            setStatus(MediaUtils::Error);
            emit newLog(output, LogUtils::Critical);
        }
    }
}

FFColorProfile *FFmpegRenderer::workingColorProfile() const
{
    return _workingColorProfile;
}

void FFmpegRenderer::setWorkingColorProfile(FFColorProfile *workingColorProfile)
{
    _workingColorProfile = workingColorProfile;
}
