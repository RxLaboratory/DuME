#include "videoinfo.h"

VideoInfo::VideoInfo(QObject *parent) : QObject(parent)
{
    FFmpeg * ffmpeg = FFmpeg::instance();
    _id = -1;
    _quality = -1;
    _encodingSpeed = -1;
    _profile = nullptr;
    _level = "";
    _tuning = nullptr;
    _bitrateType = MediaUtils::BitrateType::VBR;
    _pixAspect = 1;
    _pixFormat = ffmpeg->pixFormat("");
    _bitrate = 0;
    _framerate = 0.0;
    _height = 0;
    _width = 0;
    _codec = ffmpeg->videoEncoder("");
    _language = new FFLanguage("");
    _colorPrimaries = ffmpeg->colorPrimary("");
    _colorTRC = ffmpeg->colorTRC("");
    _colorSpace = ffmpeg->colorSpace("");
    _colorRange = ffmpeg->colorRange("");
    _premultipliedAlpha = true;
    _topCrop = 0;
    _bottomCrop = 0;
    _leftCrop = 0;
    _rightCrop = 0;
    _cropWidth = 0;
    _cropHeight = 0;
    _cropUseSize = false;
    _lut = "";
    _deinterlace = false;
    _deinterlaceParity = MediaUtils::AutoParity;
    _intra = false;
    _lossless = false;
    _speed = 1.0;
    _speedAlgorithm = ffmpeg->motionInterpolationAlgorithm("");
    _speedEstimationMode = ffmpeg->motionEstimationMode("");
    _speedInterpolationMode = MediaUtils::NoMotionInterpolation;
    _sceneDetection = true;
}

VideoInfo::VideoInfo(QJsonObject obj, QObject *parent) : QObject(parent)
{
    _language = nullptr;
    _id = -1;
    setCodec( obj.value("codec").toObject(), true);
    setQuality( obj.value("quality").toInt(-1), true);
    setEncodingSpeed( obj.value("encodingSpeed").toInt(-1), true);
    setProfile( obj.value("profile").toObject(), true);
    setLevel( obj.value("level").toString(), true);
    setTuning( obj.value("tuning").toObject(), true);
    setBitrateType( obj.value("bitrateType").toString("VBR"), true);
    setPixAspect( obj.value("pixAspect").toDouble(1), true);
    setPixFormat( obj.value("pixFormat").toObject(), true);
    setPremultipliedAlpha( obj.value("premultipliedAlpha").toBool(true), true);
    setBitrate( obj.value("bitrate").toInt(0), true);
    setFramerate( obj.value("framerate").toDouble(0.0), true);
    setHeight( obj.value("height").toInt(0), true);
    setWidth( obj.value("width").toInt(0), true);
    setLanguage( obj.value("language").toObject().value("name").toString(), true);
    setColorPrimaries( obj.value("colorPrimaries").toString(), true);
    setColorTRC( obj.value("colorTRC").toString(), true);
    setColorSpace( obj.value("colorSpace").toString(), true);
    setColorRange( obj.value("colorRange").toString(), true);
    int t = obj.value("topCrop").toInt(0);
    int b = obj.value("bottomCrop").toInt(0);
    int l = obj.value("leftCrop").toInt(0);
    int r = obj.value("rightCrop").toInt(0);
    int w = obj.value("cropWidth").toInt(0);
    int h = obj.value("cropHeight").toInt(0);
    setCrop(w,h, true);
    setCrop(t, b, l, r, true);
    setCropUseSize( obj.value("cropUseSize").toBool(false), true);
    setLut(obj.value("lut").toString(""), true);
    setDeinterlace( obj.value("deinterlace").toBool(false), true);
    setDeinterlaceParity( MediaUtils::DeinterlaceParityFromString( obj.value("deinterlaceParity").toString("Auto") ), true);
    setIntra( obj.value("intra").toBool(false), true);
    setLossless( obj.value("lossless").toBool(false), true);
    setSpeed( obj.value("speed").toDouble(1.0), true);
    setSpeedAlgorithm( obj.value("speedAlgorithm").toObject(), true);
    setSpeedEstimationMode( obj.value("speedEstimationMode").toObject(), true);
    setSpeedInterpolationMode( MediaUtils::MotionInterpolationModeFromString( obj.value("speedInterpolationMode").toString("NoMotionInterpolation")), true);
    setSceneDetection( obj.value("sceneDetection").toBool(true), true);
}

void VideoInfo::copyFrom(VideoInfo *other, bool silent)
{
    _id = other->id();
    _quality = other->quality();
    _encodingSpeed = other->encodingSpeed();
    _profile = other->profile();
    _level = other->level();
    _tuning = other->tuning();
    _bitrateType = other->bitrateType();
    _pixAspect = other->pixAspect();
    _pixFormat = other->pixFormat();
    _bitrate = other->bitrate();
    _framerate = other->framerate();
    _height = other->height();
    _width = other->width();
    _codec = other->codec();
    delete _language;
    _language = other->language();
    _colorPrimaries = other->colorPrimaries();
    _colorTRC = other->colorTRC();
    _colorSpace = other->colorSpace();
    _colorRange = other->colorRange();
    _premultipliedAlpha = other->premultipliedAlpha();
    _topCrop = other->topCrop();
    _bottomCrop = other->bottomCrop();
    _leftCrop = other->leftCrop();
    _rightCrop = other->rightCrop();
    _cropHeight = other->cropHeight();
    _cropWidth = other->cropWidth();
    _cropUseSize = other->cropUseSize();
    _lut = other->lut();
    _deinterlace = other->deinterlace();
    _deinterlaceParity = other->deinterlaceParity();
    _intra = other->intra();
    _lossless = other->lossless();
    _speed = other->speed();
    _speedAlgorithm = other->speedAlgorithm();
    _speedEstimationMode = other->speedEstimationMode();
    _speedInterpolationMode = other->speedInterpolationMode();
    _sceneDetection = other->sceneDetection();
    if(!silent) emit changed();
}

bool VideoInfo::isCopy()
{
    return _codec->name() == "copy";
}

QJsonObject VideoInfo::toJson()
{
    QJsonObject vStream;
    vStream.insert("quality", _quality);
    vStream.insert("encodingSpeed", _encodingSpeed);
    vStream.insert("profile", _profile->toJson() );
    vStream.insert("level", _level);
    vStream.insert("tuning", _tuning->toJson());
    if (_bitrateType == MediaUtils::BitrateType::CBR ) vStream.insert("bitrateType", "CBR");
    else vStream.insert("bitrateType", "VBR");
    vStream.insert("pixAspect", _pixAspect);
    vStream.insert("pixFormat", _pixFormat->toJson() );
    vStream.insert("premultipliedAlpha", _premultipliedAlpha);
    vStream.insert("bitrate", _bitrate);
    vStream.insert("framerate", _framerate);
    vStream.insert("height", _height);
    vStream.insert("width", _width);
    vStream.insert("codec", _codec->toJson() );
    vStream.insert("language", _language->toJson());
    vStream.insert("colorPrimaries", _colorPrimaries->toJson());
    vStream.insert("colorTRC", _colorTRC->toJson());
    vStream.insert("colorSpace", _colorSpace->toJson());
    vStream.insert("colorRange", _colorRange->toJson());
    vStream.insert("topCrop", _topCrop);
    vStream.insert("bottomCrop", _bottomCrop);
    vStream.insert("leftCrop", _leftCrop);
    vStream.insert("rightCrop", _rightCrop);
    vStream.insert("cropHeight", _cropHeight);
    vStream.insert("cropHWidth", _cropWidth);
    vStream.insert("cropUseSize", _cropUseSize);
    vStream.insert("lut", _lut);
    vStream.insert("deinterlace", _deinterlace);
    vStream.insert("deinterlaceParity", MediaUtils::DeinterlaceParityToString(_deinterlaceParity));
    vStream.insert("intra", _intra);
    vStream.insert("lossless", _lossless);
    vStream.insert("speed", _speed);
    vStream.insert("speedAlgorithm", _speedAlgorithm->toJson());
    vStream.insert("speedEstimationMode", _speedEstimationMode->toJson());
    vStream.insert("speedInterpolationMode", MediaUtils::MotionInterpolationModeToString(_speedInterpolationMode));
    vStream.insert("sceneDetection", _sceneDetection);
    return vStream;
}

QString VideoInfo::getDescription()
{
    QString mediaInfoString;
    mediaInfoString += "Video stream";
    if (_id >= 0) mediaInfoString += " #" + QString::number( _id ) + ":";

    if ( _language->name() != "") mediaInfoString += "\nVideo language: " + _language->prettyName();
    mediaInfoString += "\nVideo codec: ";
    FFCodec *vc = _codec;
    mediaInfoString += vc->prettyName();

    if ( vc->name() == "copy" ) return mediaInfoString;

    if (!cropUseSize())
    {
        if ( _topCrop != 0 || _bottomCrop != 0 || _leftCrop != 0 || _rightCrop != 0 )
        {
            mediaInfoString += "\nCrop margins:\n- Top: " + QString::number(_topCrop) + "px";
            mediaInfoString += "\n- Bottom: " + QString::number(_bottomCrop) + "px";
            mediaInfoString += "\n- Left: " + QString::number(_leftCrop) + "px";
            mediaInfoString += "\n- Right: " + QString::number(_rightCrop) + "px";
        }
    }
    else
    {
        if ( _cropWidth != 0 || _cropHeight != 0 )
        {
            mediaInfoString += "\nCentered crop:";
            mediaInfoString += "\n- Width: " + QString::number(_cropWidth) + "px";
            mediaInfoString += "\n- Height: " + QString::number(_cropHeight) + "px";
        }
    }
    if ( _width !=0 || _height != 0 ) mediaInfoString += "\nResolution: " + QString::number( _width ) + "px x " + QString::number( _height ) + "px";
    if ( aspect() != 0 ) mediaInfoString += "\nVideo Aspect: " + QString::number( int( aspect()*100+0.5 ) / 100.0) + ":1";
    if ( _framerate != 0 ) mediaInfoString += "\nFramerate: " + QString::number( _framerate ) + " fps";
    if (_profile) if ( _profile->name() != "") mediaInfoString += "\nProfile: " + _profile->prettyName();
    if ( _level != "") mediaInfoString += "\nLevel: " + _level;
    if (_tuning) if ( _tuning->name() != "") mediaInfoString += "\nFine tune: " + _tuning->prettyName();
    if ( _bitrate != 0 ) {
        mediaInfoString += "\nBitrate: " + MediaUtils::bitrateString(_bitrate);
        if ( _bitrateType == MediaUtils::BitrateType::CBR) mediaInfoString += " (CBR)";
    }
    if ( _quality >= 0) mediaInfoString += "\nQuality: " + QString::number( _quality ) + "%";
     if ( _encodingSpeed >= 0) mediaInfoString += "\nEncoding speed: " + QString::number( _encodingSpeed ) + "%";
    mediaInfoString += "\nPixel Aspect: " + QString::number( int(_pixAspect*100+0.5)/ 100.0) + ":1";
    if (canHaveAlpha()) mediaInfoString += "\nCan have alpha: yes";
    else mediaInfoString += "\nCan have alpha: no";
    FFPixFormat *pf =_pixFormat;
    if ( pf->name() == "" ) pf = defaultPixFormat();
    if (pf->hasAlpha())
    {
        mediaInfoString += "\nAlpha: yes";
        if (!_premultipliedAlpha) mediaInfoString += " (Unmultiply)";
    }
    else mediaInfoString += "\nAlpha: no";
    mediaInfoString += "\nPixel Format: " + pf->prettyName();
    if ( _colorTRC->name() == "iec61966_2_1" && _colorPrimaries->name() == "bt709" && _colorSpace->name() == "rgb")
    {
        mediaInfoString += "\nColor profile: sRGB";
    }
    else if ( _colorTRC->name() == "bt709" && _colorPrimaries->name() == "bt709" && _colorSpace->name() == "bt709")
    {
        mediaInfoString += "\nColor profile: HD video, BT.709";
    }
    else if ( _colorTRC->name() == "bt2020_10" && _colorPrimaries->name() == "bt2020" && _colorSpace->name() == "bt2020_cl")
    {
        mediaInfoString += "\nColor profile: UHD video, BT.2020 10 bits";
    }
    else if ( _colorTRC->name() == "bt2020_12" && _colorPrimaries->name() == "bt2020" && _colorSpace->name() == "bt2020_cl")
    {
        mediaInfoString += "\nColor profile: UHD HDR video, BT.2020 12 bits";
    }
    else
    {
        if ( _colorSpace->name() != "") mediaInfoString += "\nColor space: " + _colorSpace->prettyName();
        if ( _colorPrimaries->name() != "") mediaInfoString += "\nColor primaries: " + _colorPrimaries->prettyName();
        if ( _colorTRC->name() != "") mediaInfoString += "\nColor transfer function: " + _colorTRC->prettyName();
    }
    if ( _colorRange->name() != "" ) mediaInfoString += "\nColor range: " + _colorRange->prettyName();
    if ( _speed != 1.0)
    {
        mediaInfoString += "\nSpeed multiplicator: " + QString::number(_speed) + "x";
        mediaInfoString += "\nMotion interpolation: " + MediaUtils::MotionInterpolationModeToString(_speedInterpolationMode );
        if (_speedInterpolationMode == MediaUtils::MCIAO || _speedInterpolationMode == MediaUtils::MCIO)
        {
            mediaInfoString += "\nMotion estimation: " + _speedEstimationMode->prettyName();
            mediaInfoString += "\nMotion interpolation algorithm: " + _speedAlgorithm->prettyName();
        }
        mediaInfoString += "\nDetect scenes/shots: ";
        if (_sceneDetection) mediaInfoString += "yes";
        else mediaInfoString += "no";
    }
    if ( _lut != "") mediaInfoString += "\nLUT File: " + QDir::toNativeSeparators(_lut);
    if ( _deinterlace )
    {
        mediaInfoString += "\nDeinterlace: " + MediaUtils::DeinterlaceParityToString(_deinterlaceParity);
    }
    if (_intra)
    {
        mediaInfoString += "\nIntra-Frame";
    }
    if (_lossless)
    {
        mediaInfoString += "\nLossless compression";
    }

    return mediaInfoString;
}

int VideoInfo::quality() const
{
    return _quality;
}

void VideoInfo::setQuality(int quality, bool silent)
{
    _quality = quality;
    if(!silent) emit changed();
}

FFBaseObject *VideoInfo::profile() const
{
    return _profile;
}

void VideoInfo::setProfile(FFBaseObject *profile, bool silent)
{
    _profile = profile;
    if(!silent) emit changed();
}

void VideoInfo::setProfile(QString name, bool silent)
{
    if (_codec) setProfile( _codec->profile( name ), silent );
}

void VideoInfo::setProfile(QJsonObject obj, bool silent)
{
    setProfile( obj.value("name").toString(), silent );
}

float VideoInfo::aspect() const
{
    if (_height == 0) return 0;
    float aspect = float(_width) / float(_height);
    aspect = aspect * _pixAspect;
    return aspect;
}

float VideoInfo::pixAspect() const
{
    return _pixAspect;
}

void VideoInfo::setPixAspect(float pixAspect, bool silent)
{
    _pixAspect = pixAspect;
    if(!silent) emit changed();
}

FFPixFormat *VideoInfo::pixFormat() const
{
    return _pixFormat;
}

void VideoInfo::setPixFormat(FFPixFormat *pixFormat, bool silent)
{
    if (pixFormat->name() == "") pixFormat = defaultPixFormat();
    _pixFormat = pixFormat;
    if (_pixFormat->colorSpace() != FFPixFormat::YUV) setColorRange("");
    if(!silent) emit changed();
}

void VideoInfo::setPixFormat(QString name, bool silent)
{
    setPixFormat( FFmpeg::instance()->pixFormat( name ), silent );
}

void VideoInfo::setPixFormat(QJsonObject obj, bool silent)
{
    setPixFormat( obj.value("name").toString(), silent );
}

FFPixFormat *VideoInfo::defaultPixFormat() const
{
    return _codec->defaultPixFormat();
}

qint64 VideoInfo::bitrate() const
{
    return _bitrate;
}

void VideoInfo::setBitrate(const qint64 &bitrate, bool silent)
{
    _bitrate = bitrate;
    if(!silent) emit changed();
}

double VideoInfo::framerate() const
{
    return _framerate;
}

void VideoInfo::setFramerate(double framerate, bool silent)
{
    _framerate = framerate;
    if(!silent) emit changed();
}

int VideoInfo::height() const
{
    return _height;
}

void VideoInfo::setHeight(int height, bool silent)
{
    _height = height;
    if(!silent) emit changed();
}

int VideoInfo::width() const
{
    return _width;
}

void VideoInfo::setWidth(int width, bool silent)
{
    _width = width;
    if(!silent) emit changed();
}

FFCodec *VideoInfo::codec() const
{
    return _codec;
}

FFLanguage *VideoInfo::language() const
{
    return _language;
}

void VideoInfo::setLanguage(const QString &languageId, bool silent)
{
    delete _language;
    _language = new FFLanguage( languageId );
    if(!silent) emit changed();
}

void VideoInfo::setColorPrimaries(QString primaries, bool silent)
{
    setColorPrimaries( FFmpeg::instance()->colorPrimary(primaries), silent );
}

void VideoInfo::setColorTRC(QString tRC, bool silent)
{
    setColorTRC( FFmpeg::instance()->colorTRC(tRC), silent );
}

void VideoInfo::setColorSpace(QString space, bool silent)
{
    setColorSpace( FFmpeg::instance()->colorSpace(space), silent);
}

void VideoInfo::setColorRange(QString range, bool silent)
{
    setColorRange( FFmpeg::instance()->colorRange(range), silent);
}

void VideoInfo::setColorPrimaries(FFBaseObject *primaries, bool silent)
{
    _colorPrimaries = primaries;
    if(!silent) emit changed();
}

void VideoInfo::setColorTRC(FFBaseObject *tRC, bool silent)
{
    _colorTRC = tRC;
    if(!silent) emit changed();
}

void VideoInfo::setColorSpace(FFBaseObject *space, bool silent)
{
    _colorSpace = space;
    if(!silent) emit changed();
}

void VideoInfo::setColorRange(FFBaseObject *range, bool silent)
{
    if (_pixFormat->colorSpace() == FFPixFormat::YUV)
        _colorRange = range;
    else
        _colorRange = FFmpeg::instance()->colorRange("");
    if(!silent) emit changed();
}

void VideoInfo::setColorProfile(FFColorProfile *colorProfile, bool silent)
{
    _colorPrimaries = colorProfile->primaries();
    _colorTRC = colorProfile->trc();
    _colorSpace = colorProfile->space();
    _colorRange = colorProfile->range();
    if(!silent) emit changed();
}

void VideoInfo::setColorProfile(QString profile, bool silent)
{
    setColorProfile( FFmpeg::instance()->colorProfile(profile), silent );
}

bool VideoInfo::setAlpha(bool alpha, bool silent)
{
    //select a pixfmt which has an alpha, the closest to the current one (or default)
    FFPixFormat *pf = _pixFormat;
    if ( pf->name() == "" ) pf = _codec->defaultPixFormat();
    if ( pf->name() == "" && _codec->pixFormats().count() > 0) pf = _codec->pixFormats()[0];

    _pixFormat = _codec->pixFormatWithAlpha(pf, alpha);

    if(!silent) emit changed();
    return true;
}

bool VideoInfo::hasAlpha()
{
    FFPixFormat *pf = _pixFormat;
    if (pf->name() == "") pf = _codec->defaultPixFormat();

    return pf->hasAlpha();
}

bool VideoInfo::canHaveAlpha()
{
    foreach (FFPixFormat *pf, _codec->pixFormats())
    {
        if (pf->hasAlpha()) return true;
    }

    return false;
}

int VideoInfo::id() const
{
    return _id;
}

void VideoInfo::setId(int id, bool silent)
{
    _id = id;
    if(!silent) emit changed();
}

QString VideoInfo::level() const
{
    return _level;
}

void VideoInfo::setLevel(const QString &level, bool silent)
{
    _level = level;
    if(!silent) emit changed();
}

bool VideoInfo::premultipliedAlpha() const
{
    return _premultipliedAlpha;
}

void VideoInfo::setPremultipliedAlpha(bool premultipliedAlpha, bool silent)
{
    _premultipliedAlpha = premultipliedAlpha;
    if(!silent) emit changed();
}

FFBaseObject *VideoInfo::tuning() const
{
    return _tuning;
}

void VideoInfo::setTuning(FFBaseObject *tuning, bool silent)
{
    _tuning = tuning;
    if(!silent) emit changed();
}

void VideoInfo::setTuning(QString tuning, bool silent)
{
    if (_codec) setTuning(_codec->tuning(tuning), silent);
}

void VideoInfo::setTuning(QJsonObject tuning, bool silent)
{
    setTuning(tuning.value("name").toString(), silent);
}

MediaUtils::BitrateType VideoInfo::bitrateType() const
{
    return _bitrateType;
}

void VideoInfo::setBitrateType(const MediaUtils::BitrateType &bitrateType, bool silent)
{
    _bitrateType = bitrateType;
    if(!silent) emit changed();
}

void VideoInfo::setBitrateType(QString bitrateType, bool silent)
{
    if (bitrateType.toLower() == "cbr") setBitrateType(MediaUtils::BitrateType::CBR, silent);
    else setBitrateType(MediaUtils::BitrateType::VBR, silent);
}

void VideoInfo::setCrop(int top, int bottom, int left, int right, bool silent)
{
    _topCrop = top;
    _bottomCrop = bottom;
    _leftCrop = left;
    _rightCrop = right;

    if(!silent) emit changed();
}

void VideoInfo::setCrop(int width, int height, bool silent)
{
    _cropWidth = width;
    _cropHeight = height;

    if(!silent) emit changed();
}

int VideoInfo::topCrop() const
{
    return _topCrop;
}

int VideoInfo::bottomCrop() const
{
    return _bottomCrop;
}

int VideoInfo::leftCrop() const
{
    return _leftCrop;
}

int VideoInfo::rightCrop() const
{
    return _rightCrop;
}

int VideoInfo::cropHeight() const
{
    return _cropHeight;
}

int VideoInfo::cropWidth() const
{
    return _cropWidth;
}

bool VideoInfo::cropUseSize() const
{
    return _cropUseSize;
}

void VideoInfo::setCropUseSize(bool cropUseSize, bool silent)
{
    _cropUseSize = cropUseSize;

    if(!silent) emit changed();
}

QString VideoInfo::lut() const
{
    return _lut;
}

void VideoInfo::setLut(const QString &lut, bool silent)
{
    _lut = lut;

    if(!silent) emit changed();
}

bool VideoInfo::deinterlace() const
{
    return _deinterlace;
}

void VideoInfo::setDeinterlace(bool deinterlace, bool silent)
{
    _deinterlace = deinterlace;

    if(!silent) emit changed();
}

MediaUtils::DeinterlaceParity VideoInfo::deinterlaceParity() const
{
    return _deinterlaceParity;
}

void VideoInfo::setDeinterlaceParity(const MediaUtils::DeinterlaceParity &deinterlaceParity, bool silent)
{
    _deinterlaceParity = deinterlaceParity;

    if(!silent) emit changed();
}

bool VideoInfo::intra() const
{
    return _intra;
}

void VideoInfo::setIntra(bool intra, bool silent)
{
    _intra = intra;
    if(!silent) emit changed();
}

bool VideoInfo::lossless() const
{
    return _lossless;
}

void VideoInfo::setLossless(bool lossless, bool silent)
{
    _lossless = lossless;
    if(!silent) emit changed();
}

float VideoInfo::speed() const
{
    return _speed;
}

void VideoInfo::setSpeed(float speed, bool silent)
{
    _speed = speed;
    if(!silent) emit changed();
}

MediaUtils::MotionInterpolationMode VideoInfo::speedInterpolationMode() const
{
    return _speedInterpolationMode;
}

void VideoInfo::setSpeedInterpolationMode(const MediaUtils::MotionInterpolationMode &speedInterpolationMode, bool silent)
{
    _speedInterpolationMode = speedInterpolationMode;
    if(!silent) emit changed();
}

FFBaseObject *VideoInfo::speedEstimationMode() const
{
    return _speedEstimationMode;
}

void VideoInfo::setSpeedEstimationMode(FFBaseObject *speedEstimationMode, bool silent)
{
    _speedEstimationMode = speedEstimationMode;
    if(!silent) emit changed();
}

void VideoInfo::setSpeedEstimationMode(QString speedEstimationMode, bool silent)
{
    setSpeedEstimationMode( FFmpeg::instance()->motionEstimationMode(speedEstimationMode), silent);
}

void VideoInfo::setSpeedEstimationMode(QJsonObject speedEstimationMode, bool silent)
{
    setSpeedEstimationMode(speedEstimationMode.value("name").toString(), silent);
}

FFBaseObject *VideoInfo::speedAlgorithm() const
{
    return _speedAlgorithm;
}

void VideoInfo::setSpeedAlgorithm(FFBaseObject *speedAlgorithm, bool silent)
{
    _speedAlgorithm = speedAlgorithm;
    if(!silent) emit changed();
}

void VideoInfo::setSpeedAlgorithm(QString speedAlgorithm, bool silent)
{
    setSpeedAlgorithm( FFmpeg::instance()->motionInterpolationAlgorithm(speedAlgorithm), silent);
}

void VideoInfo::setSpeedAlgorithm(QJsonObject speedAlgorithm, bool silent)
{
    setSpeedAlgorithm(speedAlgorithm.value("name").toString(), silent);
}

bool VideoInfo::sceneDetection() const
{
    return _sceneDetection;
}

void VideoInfo::setSceneDetection(bool sceneDetection, bool silent)
{
    _sceneDetection = sceneDetection;
    if(!silent) emit changed();
}

int VideoInfo::encodingSpeed() const
{
    return _encodingSpeed;
}

void VideoInfo::setEncodingSpeed(int encodingSpeed, bool silent)
{
    _encodingSpeed = encodingSpeed;
    if(!silent) emit changed();
}

FFBaseObject *VideoInfo::colorPrimaries()
{
    return _colorPrimaries;
}

FFBaseObject *VideoInfo::colorTRC()
{
    return _colorTRC;
}

FFBaseObject *VideoInfo::colorSpace()
{
    return _colorSpace;
}

FFBaseObject *VideoInfo::colorRange()
{
    return _colorRange;
}

void VideoInfo::setCodec(FFCodec *codec, bool silent)
{
    qDebug() << "Codec changed to " + codec->name();
    _codec = codec;
    _tuning = codec->tuning("");
    if (codec->useBitrateType())
    {
        _bitrateType = MediaUtils::BitrateType::VBR;
    }
    if (codec->isLossless() && !codec->isLossy()) _lossless = true;
    if(!silent) emit changed();
}

void VideoInfo::setCodec(QString name, bool silent)
{
    setCodec( FFmpeg::instance()->videoEncoder(name), silent);
}

void VideoInfo::setCodec(QJsonObject obj, bool silent)
{
    setCodec( obj.value("name").toString(), silent );
}
