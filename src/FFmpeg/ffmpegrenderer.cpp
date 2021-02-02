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

    //output checks
    MediaInfo *o = _job->getOutputMedias()[0];
    FFPixFormat *outputPixFmt = nullptr;
    FFPixFormat::ColorSpace outputColorSpace = FFPixFormat::OTHER;

    if (o->hasVideo())
    {
        outputPixFmt = o->videoStreams()[0]->pixFormat();
        if (outputPixFmt->name() == "") outputPixFmt = o->videoStreams()[0]->defaultPixFormat();
        if (outputPixFmt->name() == "") outputPixFmt = o->defaultPixFormat();
        if (outputPixFmt->name() != "") outputColorSpace = outputPixFmt->colorSpace();
    }

    //input checks
    bool exrInput = false;
    MediaInfo *i = _job->getInputMedias()[0];
    if (i->hasVideo()) exrInput = i->videoStreams()[0]->codec()->name() == "exr";

    //some values to be passed from the input to the output
    double inputFramerate = 0.0;

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
                else arguments << "-framerate" << "24";
                inputFileName = input->ffmpegSequenceName();
            }

            //add color management
            FFPixFormat::ColorSpace inputColorSpace = FFPixFormat::OTHER;
            FFPixFormat *inputPixFmt = stream->pixFormat();
            if (inputPixFmt->name() == "") inputPixFmt = stream->defaultPixFormat();
            if (inputPixFmt->name() == "") inputPixFmt = input->defaultPixFormat();
            inputColorSpace = inputPixFmt->colorSpace();

            bool convertToYUV = outputColorSpace == FFPixFormat::YUV && inputColorSpace != FFPixFormat::YUV  && input->hasVideo() ;

            if (stream->colorTRC()->name() != "" ) arguments << "-color_trc" << stream->colorTRC()->name();
            else if ( convertToYUV ) arguments << "-color_trc" << "bt709";
            if (stream->colorRange()->name() != "") arguments << "-color_range" << stream->colorRange()->name();
            else if ( convertToYUV ) arguments << "-color_range" << "tv";
            if (stream->colorPrimaries()->name() != "") arguments << "-color_primaries" << stream->colorPrimaries()->name();
            else if ( convertToYUV ) arguments << "-color_primaries" << "bt709";
            if (stream->colorSpace()->name() != "") arguments << "-colorspace" << stream->colorSpace()->name();
            else if ( convertToYUV ) arguments << "-colorspace" << "bt709";
        }

        //time range
        if (input->inPoint() != 0.0) arguments << "-ss" << QString::number(input->inPoint());
        if (input->outPoint() != 0.0) arguments << "-to" << QString::number(input->outPoint());

        //add input file
        arguments << "-i" << QDir::toNativeSeparators(inputFileName);
    }
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
                if (stream->framerate() != 0.0) arguments << "-r" << QString::number(stream->framerate());

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

                //color
                //add color management
                if (stream->colorTRC()->name() != "") arguments << "-color_trc" << stream->colorTRC()->name();
                if (stream->colorRange()->name() != "") arguments << "-color_range" << stream->colorRange()->name();
                if (stream->colorTRC()->name() != "") arguments << "-color_primaries" << stream->colorPrimaries()->name();
                if (stream->colorSpace()->name() != "") arguments << "-colorspace" << stream->colorSpace()->name();

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

                //LUT for input EXR
                if (exrInput) filterChain << "lutrgb=r=gammaval(0.416666667):g=gammaval(0.416666667):b=gammaval(0.416666667)";

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

    foreach (MediaInfo *m, _job->getInputMedias())
    {
        if (m->hasVideo())
        {
            VideoInfo *stream = m->videoStreams()[0];
            this->setNumFrames( int( m->duration() * stream->framerate() ) );
            this->setFrameRate( stream->framerate() );
            break;
        }
    }

    this->start( arguments );
    return true;
}

void FFmpegRenderer::readyRead(QString output)
{
    QRegularExpression reProgress("(?:frame= *(\\d+).*fps= *(\\d+).*)?size= *(?:(\\d+)kB)?.*time=(\\d\\d:\\d\\d:\\d\\d.\\d\\d).*bitrate= *(?:(\\d+).\\d+kbits)?.*speed= *(\\d+.\\d*)x");
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

    emit console(output);
}
