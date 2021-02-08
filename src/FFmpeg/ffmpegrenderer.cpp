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

}

bool FFmpegRenderer::launchJob()
{
    qDebug() << "Launching FFMpeg Job";
    setStatus( MediaUtils::Launching );

    //generate arguments
    QStringList arguments;
    arguments << "-loglevel" << "error" << "-stats" << "-y";

    // Check if we need to convert colors
    bool convertColors = colorManagement();

    //some values needed later to get from the input
    double inputFramerate = 0.0;
    double totalDuration = 0.0;

    //add inputs
    foreach(MediaInfo *input, _job->getInputMedias())
    {
        QString inputFileName = input->fileName();
        //add custom options
        foreach(QStringList option,input->ffmpegOptions())
        {
            arguments << option[0];
            if (option.count() > 1)
            {
                if (option[1] != "") arguments << option[1];
            }
        }

        if (input->hasVideo())
        {
            VideoInfo *stream = input->videoStreams()[0];

            if (stream->framerate() != 0.0) inputFramerate = stream->framerate();

            //add sequence options
            if (input->isSequence())
            {
                arguments << "-start_number" << QString::number(input->startNumber());
                if (stream->framerate() != 0.0) arguments << "-framerate" << QString::number(stream->framerate());
                else
                {
                    arguments << "-framerate" << "24";
                    inputFramerate = 24.0;
                }
                inputFileName = input->ffmpegSequenceName();
            }

            // COLOR MANAGEMENT

            // Get the default Color profile
            FFColorProfile *profile = input->defaultColorProfile();
            QString profileName = profile->name();

            // Set input color interpretation
            if (stream->colorTRC()->name() != "" ) arguments << "-color_trc" << stream->colorTRC()->name();
            else if ( convertColors && profileName != "" ) arguments << "-color_trc" << profile->trc()->name();

            if (stream->colorRange()->name() != "") arguments << "-color_range" << stream->colorRange()->name();
            else if ( convertColors && profileName != "" ) arguments << "-color_range" << profile->range()->name();

            if (stream->colorPrimaries()->name() != "") arguments << "-color_primaries" << stream->colorPrimaries()->name();
            else if ( convertColors && profileName != "" ) arguments << "-color_primaries" << profile->primaries()->name();

            if (stream->colorSpace()->name() != "") arguments << "-colorspace" << stream->colorSpace()->name();
            else if ( convertColors && profileName != "" ) arguments << "-colorspace" << profile->space()->name();
        }

        //get duration
        if (input->inPoint() != 0.0 || input->outPoint() != 0.0)
        {
            double test = input->outPoint() - input->inPoint();
            if (test > totalDuration) totalDuration = test;
        }
        else if (input->duration() > totalDuration) totalDuration = input->duration();
        else if (input->isSequence())
        {
            if (inputFramerate == 0.0) inputFramerate = 24.0;
            double test = input->frames().count() / inputFramerate;
            if (test > totalDuration) totalDuration = test;
        }

        //time range
        if (input->inPoint() != 0.0) arguments << "-ss" << QString::number(input->inPoint());
        if (input->outPoint() != 0.0) arguments << "-to" << QString::number(input->outPoint());

        //add input file
        arguments << "-i" << QDir::toNativeSeparators(inputFileName);
    }

    //some values needed later to get from the output
    double speedMultiplicator = 1.0;
    double outputFrameRate = 0.0;

    //add outputs
    foreach(MediaInfo *output, _job->getOutputMedias())
    {
        //maps
        foreach (StreamReference map, output->maps())
        {
            int mediaId = map.mediaId();
            int streamId = map.streamId();
            if (mediaId >= 0 && streamId >= 0) arguments << "-map" << QString::number( mediaId ) + ":" + QString::number( streamId );
        }

        //muxer
        QString muxer = output->muxer()->name();
        if (output->muxer()->isSequence()) muxer = "image2";

        if (muxer != "") arguments << "-f" << muxer;

        //add custom options
        foreach(QStringList option, output->ffmpegOptions())
        {
            QString opt = option[0];
            //Ignore video filters
            if (opt == "-filter:v" || opt == "-vf") continue;
            arguments << opt;
            if (option.count() > 1)
            {
                if (option[1] != "") arguments << option[1];
            }
        }

        //video
        if (output->hasVideo())
        {
            VideoInfo *stream = output->videoStreams()[0];

            FFCodec *vc = stream->codec();
            if (vc->name() != "") arguments << "-c:v" << vc->name();
            else vc = output->defaultVideoCodec();

            if (vc->name() != "copy")
            {
                //bitrate
                int bitrate = int( stream->bitrate() );
                if (bitrate != 0) arguments << "-b:v" << QString::number(bitrate);

                //bitrate type
                if (vc->useBitrateType() && stream->bitrateType() == MediaUtils::BitrateType::CBR)
                {
                    arguments << "-minrate" << QString::number(bitrate);
                    arguments << "-maxrate" << QString::number(bitrate);
                    arguments << "-bufsize" << QString::number(bitrate*2);
                }

                //framerate
                if (stream->framerate() != 0.0)
                {
                    arguments << "-r" << QString::number(stream->framerate());
                    outputFrameRate = stream->framerate();
                }

                //loop (gif)
                if (vc->name() == "gif")
                {
                    int loop = output->loop();
                    arguments << "-loop" << QString::number(loop);
                }

                //level
                if (stream->level() != "") arguments << "-level" << stream->level();

                //quality
                int quality = stream->quality();
                if ( quality >= 0 && vc->name() != "" && vc->qualityParam() != "")
                {
                    arguments << vc->qualityParam() << vc->qualityValue(quality);
                }

                //encoding speed
                int speed = stream->encodingSpeed();
                if (vc->useSpeed() && speed >= 0) arguments << vc->speedParam() << vc->speedValue(speed);

                //fine tuning
                if (vc->useTuning() && stream->tuning()->name() != "")
                {
                    arguments << "-tune" << stream->tuning()->name();
                    if (stream->tuning()->name() == "zerolatency")
                    {
                        arguments << "-movflags" << "+faststart";
                    }
                }

                //start number (sequences)
                if (muxer == "image2")
                {
                    int startNumber = output->startNumber();
                    arguments << "-start_number" << QString::number(startNumber);
                }

                //pixel format
                FFPixFormat *pixFormat = stream->pixFormat();
                QString pixFmt = pixFormat->name();
                //set default for h264 to yuv420 (ffmpeg generates 444 by default which is not standard)
                if (pixFmt == "" && (vc->name() == "h264" || vc->name() == "libx264") ) pixFmt = "yuv420p";
                if (pixFmt == "" && muxer == "mp4") pixFmt = "yuv420p";
                if (pixFmt != "") arguments << "-pix_fmt" << pixFmt;
                // video codecs with alpha need to set -auto-alt-ref to 0
                if (stream->pixFormat()->hasAlpha() && muxer != "image2") arguments << "-auto-alt-ref" << "0";

                //profile
                if (stream->profile()->name() != "" && vc->useProfile())
                {
                    QString profile = stream->profile()->name();
                    // Adjust h264 main profile
                    if ( (vc->name() == "h264" || vc->name() == "libx264") && profile == "high")
                    {
                        if (stream->pixFormat()->name() != "")
                        {
                            if (pixFormat->yuvComponentsDistribution() == "444") profile = "high444";
                            else if (pixFormat->yuvComponentsDistribution() == "422") profile = "high422";
                            else if (pixFormat->bitsPerPixel() > 12) profile = "high10"; // 12 bpp at 420 is 8bpc
                        }
                    }
                    arguments << "-profile:v" << profile;
                }

                // h265 options
                if (vc->name() == "hevc" || vc->name() == "h265" || vc->name() == "libx265")
                {
                    QStringList x265options;
                    if (stream->intra())
                    {
                        float ffmepgVersion = FFmpeg::instance()->version().left(3).toFloat() ;
                        if (ffmepgVersion >= 4.3)
                        {
                            arguments << "-g" << "1";
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
                        arguments << "-x265-params" << "\"" + x265options.join(":") + "\"";
                    }
                }

                // h264 intra
                if (vc->name() == "h264" || vc->name() == "libx264")
                {
                    if (stream->intra())
                    {
                        float ffmepgVersion = FFmpeg::instance()->version().left(3).toFloat() ;
                        if (ffmepgVersion >= 4.3)
                        {
                            arguments << "-g" << "1";
                        }
                        else
                        {
                            arguments << "-intra";
                        }
                    }

                }

                // COLOR MANAGEMENT

                // Get the default Color profile
                FFColorProfile *profile = output->defaultColorProfile();
                QString profileName = profile->name();

                // color
                // Add color management MetaData (embed profile)
                if (stream->colorConversionMode() != MediaUtils::Convert)
                {
                    if (stream->colorTRC()->name() != "" && !stream->colorTRC()->name().startsWith("-")) arguments << "-color_trc" << stream->colorTRC()->name();
                    else if (profileName != "" && convertColors) arguments << "-color_trc" << profile->trc()->name();

                    if (stream->colorRange()->name() != "") arguments << "-color_range" << stream->colorRange()->name();
                    else if ( convertColors && profileName != "" ) arguments << "-color_range" << profile->range()->name();

                    if (stream->colorPrimaries()->name() != "") arguments << "-color_primaries" << stream->colorPrimaries()->name();
                    else if ( convertColors && profileName != "" ) arguments << "-color_primaries" << profile->primaries()->name();

                    if (stream->colorSpace()->name() != "") arguments << "-colorspace" << stream->colorSpace()->name();
                    else if ( convertColors && profileName != "" ) arguments << "-colorspace" << profile->space()->name();
                }

                //b-pyramids
                //set as none to h264: not really useful (only on very static footage), but has compatibility issues
                if (vc->name() == "h264") arguments << "-x264opts" << "b_pyramid=0";

                //Vendor
                //set to ap10 with prores for improved compatibility
                if (vc->name().indexOf("prores") >= 0) arguments << "-vendor" << "ap10";

                // ============ Video filters
                QStringList filterChain;

                //unpremultiply
                bool unpremultiply = !stream->premultipliedAlpha();
                if (unpremultiply) filterChain << "unpremultiply=inplace=1";

                //deinterlace
                if (stream->deinterlace())
                {
                    QString deinterlaceOption = "yadif=parity=";
                    if (stream->deinterlaceParity() == MediaUtils::TopFieldFirst) deinterlaceOption += "0";
                    else if (stream->deinterlaceParity() == MediaUtils::BottomFieldFirst) deinterlaceOption += "1";
                    else deinterlaceOption += "-1";
                    filterChain << deinterlaceOption;
                }

                //speed
                if (stream->speed() != 1.0) filterChain << "setpts=" + QString::number(1/stream->speed()) + "*PTS";
                speedMultiplicator = stream->speed();

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
                    if (framerate == 0.0) framerate = inputFramerate;
                    if (framerate > 0.0) speedFilter += ":fps=" + QString::number(framerate);
                    speedFilter += "'";
                    filterChain << speedFilter;
                }

                //crop
                if (!stream->cropUseSize() && ( stream->topCrop() != 0 || stream->bottomCrop() != 0 || stream->leftCrop() != 0 || stream->rightCrop() != 0))
                {
                    QString left = QString::number(stream->leftCrop());
                    QString right = QString::number(stream->rightCrop());
                    QString top = QString::number(stream->topCrop());
                    QString bottom = QString::number(stream->bottomCrop());
                    filterChain << "crop=in_w-" + left + "-" + right + ":in_h-" + top + "-" + bottom + ":" + left + ":" + top;
                }
                else if (stream->cropUseSize() && ( stream->cropHeight() != 0 || stream->cropWidth() != 0) )
                {
                    int wi = stream->cropWidth();
                    int hi = stream->cropHeight();
                    QString w = QString::number( wi );
                    if (wi == 0) w = "in_w";
                    QString h = QString::number( hi );
                    if (hi == 0) h = "in_h";
                    filterChain << "crop=" + w + ":" + h;
                }

                //Collect custom
                foreach(QStringList option, output->ffmpegOptions())
                {
                    QString opt = option[0];
                    //Get video filters
                    if (opt != "-filter:v" && opt != "-vf") continue;
                    if (option.count() > 1)
                    {
                        if (option[1] != "") filterChain << option[1];
                    }
                }

                //1D / 3D LUT
                if (stream->lut() != "")
                {
                    bool filterName = "lut3d";
                    QString lutName = stream->lut();
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
                    filterChain << "lut1d='" + FFmpeg::escapeFilterOption( stream->lut().replace("\\","/") ) + "'";
                }

                //size
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
                        filterChain << "scale=" + w + ":" + h + resizeAlgo;
                        //we need to set the pixel aspect ratio back to 1:1 to force ffmpeg to stretch
                        filterChain << "setsar=1:1";
                    }
                    else if (stream->resizeMode() == MediaUtils::Crop)
                    {
                        //first resize but keeping ratio (increase)
                        filterChain << "scale=w=" + w + ":h=" + h + ":force_original_aspect_ratio=increase" + resizeAlgo;
                        //then crop what's to large
                        filterChain << "crop=" + w + ":" + h;
                    }
                    else if (stream->resizeMode() == MediaUtils::Letterbox)
                    {
                        //first resize but keeping ratio (decrease)
                        filterChain << "scale=w=" + w + ":h=" + h + ":force_original_aspect_ratio=decrease" + resizeAlgo;
                        //then pad with black bars
                        filterChain << "pad=" + w + ":" + h + ":(ow-iw)/2:(oh-ih)/2";
                    }
                }

                // COLOR MANAGEMENT

                //Color conversion
                QStringList zScaleArgs;
                QStringList colorspaceArgs;
                QStringList lutrgbArgs;
                if (stream->colorConversionMode() != MediaUtils::Embed)
                {
                    // Get TRC
                    FFColorItem *trc = stream->colorTRC();
                    if (trc->name() == "" && profile->name() != "" && convertColors) trc = profile->trc();
                    if (trc->scaleName() != "")
                    {
                        if (trc->scaleFilter() == FFColorItem::ZScale) zScaleArgs << "transfer=" + trc->scaleName();
                        else if (trc->scaleFilter() == FFColorItem::Colorspace) colorspaceArgs << "trc=" + trc->scaleName();
                        else if (trc->scaleFilter() == FFColorItem::Gamma)
                        {
                            //linearize first
                            zScaleArgs << "transfer=linear";
                            //apply gamma
                            lutrgbArgs << "r=gammaval(1/" + trc->scaleName() + ")";
                            lutrgbArgs << "g=gammaval(1/" + trc->scaleName() + ")";
                            lutrgbArgs << "b=gammaval(1/" + trc->scaleName() + ")";
                        }
                    }
                    // Get Range
                    FFColorItem *range = stream->colorRange();
                    if (range->name() == "" && profile->name() != "" && convertColors) range = profile->range();
                    if (range->scaleName() != "")
                    {
                        zScaleArgs << "range=" + range->scaleName();
                    }
                    // Get Primaries
                    FFColorItem *primaries = stream->colorPrimaries();
                    if (primaries->name() == "" && profile->name() != "" && convertColors) primaries = profile->primaries();
                    if (primaries->scaleName() != "")
                    {
                        if (stream->colorPrimaries()->scaleFilter() == FFColorItem::ZScale) zScaleArgs << "primaries=" + primaries->scaleName();
                        else if (stream->colorPrimaries()->scaleFilter() == FFColorItem::Colorspace) colorspaceArgs << "primaries=" + primaries->scaleName();
                    }
                    FFColorItem *space = stream->colorSpace();
                    if (space->name() == "" && profile->name() != "" && convertColors) space = profile->space();
                    if (space->scaleName() != "")
                    {
                        if (stream->colorSpace()->scaleFilter() == FFColorItem::ZScale) zScaleArgs << "matrix=" + space->scaleName();
                        else if (stream->colorSpace()->scaleFilter() == FFColorItem::Colorspace) colorspaceArgs << "space=" + space->scaleName();
                    }
                }
                if (zScaleArgs.count() > 0)
                {
                    zScaleArgs << "dither=ordered";
                    filterChain << "zscale=" + zScaleArgs.join(":");
                }
                if (colorspaceArgs.count() > 0)
                {
                    filterChain << "colorspace=" + colorspaceArgs.join(":");
                }
                if (lutrgbArgs.count() > 0)
                {
                    filterChain << "lutrgb=" + lutrgbArgs.join(":");
                }

                //compile filters
                if (filterChain.count() > 0) arguments << "-vf" << filterChain.join(",");
            }
        }
        else
        {
             arguments << "-vn";
        }

        //audio
        if (output->hasAudio())
        {
            AudioInfo *stream = output->audioStreams()[0];

            QString acodec = stream->codec()->name();
            if (acodec == "")

            //codec
            if (acodec != "") arguments << "-c:a" << acodec;

            if (acodec != "copy")
            {
                //bitrate
                int bitrate = int( stream->bitrate() );
                if (bitrate != 0)
                {
                    arguments << "-b:a" << QString::number(stream->bitrate());
                }

                //sampling
                int sampling = stream->samplingRate();
                if (sampling != 0)
                {
                    arguments << "-ar" << QString::number(sampling);
                }

                //sample format
                QString sampleFormat = stream->sampleFormat()->name();
                if (sampleFormat != "")
                {
                    arguments << "-sample_fmt" << sampleFormat;
                }
            }
        }
        else
        {
            //no audio
            arguments << "-an";
        }

        //file
        QString outputPath = QDir::toNativeSeparators( output->fileName() );

        //if sequence, digits
        if (output->isSequence())
        {
            outputPath = QDir::toNativeSeparators( output->ffmpegSequenceName() );
        }

        arguments << outputPath;
    }

    emit newLog("Beginning new encoding\nUsing FFmpeg commands:\n" + arguments.join(" | "));

    //launch
    this->setOutputFileName( _job->getOutputMedias()[0]->fileName() );

    if (outputFrameRate == 0.0) outputFrameRate = inputFramerate;

    if (outputFrameRate != 0.0)
    {
        this->setNumFrames( totalDuration * outputFrameRate / speedMultiplicator );
        this->setFrameRate( outputFrameRate );
    }

    this->start( arguments );
    return true;
}

bool FFmpegRenderer::colorManagement()
{
    qDebug() << "Color management";

    if (!_job) return false;

    MediaInfo *o = _job->getOutputMedias()[0];
    if (!o->hasVideo()) return false;

    VideoInfo *ov = o->videoStreams()[0];

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

    // If the color space of the output is different from the input, we do need to convert anyway.

    qDebug() << "Get the default output Color Profile";

    // We can check that from the pixel format, so first get it
    // Get the default Color profile
    FFColorProfile *outputProfile = o->defaultColorProfile();

    qDebug() << outputProfile->prettyName();

    // Now let's check if any of the input has a different input color profile
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

        qDebug() << "Input: " + inputProfile->name();
        qDebug() << "Output: " + outputProfile->name();

        // If the profile is different, there is a color conversion
        if (inputProfile->name() != outputProfile->name()) return true;
    }
    return false;
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
