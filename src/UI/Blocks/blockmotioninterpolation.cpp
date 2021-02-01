#include "blockmotioninterpolation.h"

BlockMotionInterpolation::BlockMotionInterpolation(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create motion interpolation block";
#endif
    setType(Type::Video);
    setupUi(this);

    _freezeUI = true;

    interpolationBox->addItem("Duplicate frames");
    interpolationBox->addItem("Blend frames");
    interpolationBox->addItem("Motion compensated - Overlapped");
    interpolationBox->addItem("Motion compensated - Adaptive");
    interpolationBox->setCurrentIndex(0);

    foreach(FFBaseObject *me, FFmpeg::instance()->motionEstimationModes())
    {
        estimationBox->addItem(me->prettyName(), me->name());
    }
    estimationBox->setCurrentIndex(0);

    foreach(FFBaseObject *mia, FFmpeg::instance()->motionInterpolationAlgorithms())
    {
        algorithmBox->addItem(mia->prettyName(), mia->name());
    }
    algorithmBox->setCurrentIndex(0);

    updateInterpolationUI();

    _freezeUI = false;
}

void BlockMotionInterpolation::activate(bool blockEnabled)
{
    if (!blockEnabled)
    {
        _mediaInfo->setVideoSpeedInterpolationMode(MediaUtils::NoMotionInterpolation);
    }
    else
    {
        int index = interpolationBox->currentIndex();
        if (index == 0) _mediaInfo->setVideoSpeedInterpolationMode(MediaUtils::DuplicateFrames);
        else if (index == 1) _mediaInfo->setVideoSpeedInterpolationMode(MediaUtils::BlendFrames);
        else if (index == 2) _mediaInfo->setVideoSpeedInterpolationMode(MediaUtils::MCIO);
        else if (index == 3) _mediaInfo->setVideoSpeedInterpolationMode(MediaUtils::MCIAO);
        else _mediaInfo->setVideoSpeedInterpolationMode(MediaUtils::NoMotionInterpolation);
        _mediaInfo->setVideoSpeedEstimationMode( estimationBox->currentData().toString() );
        _mediaInfo->setVideoSpeedAlgorithm( algorithmBox->currentData().toString() );
        _mediaInfo->setSceneDetection( sceneDetectionBox->isChecked());
    }
}

void BlockMotionInterpolation::update()
{
    VideoInfo *stream = _mediaInfo->videoStreams()[0];
    if (stream->speedInterpolationMode() == MediaUtils::DuplicateFrames) interpolationBox->setCurrentIndex(0);
    else if (stream->speedInterpolationMode() == MediaUtils::BlendFrames) interpolationBox->setCurrentIndex(1);
    else if (stream->speedInterpolationMode() == MediaUtils::MCIO) interpolationBox->setCurrentIndex(2);
    else if (stream->speedInterpolationMode() == MediaUtils::MCIAO) interpolationBox->setCurrentIndex(3);
    else interpolationBox->setCurrentIndex(-1);
    estimationBox->setCurrentData(stream->speedEstimationMode()->name());
    algorithmBox->setCurrentData(stream->speedAlgorithm()->name());
    sceneDetectionBox->setChecked(stream->sceneDetection());
}

void BlockMotionInterpolation::on_interpolationBox_currentIndexChanged(int index)
{
    updateInterpolationUI();
    if (_freezeUI) return;
    if (index == 0) _mediaInfo->setVideoSpeedInterpolationMode(MediaUtils::DuplicateFrames);
    else if (index == 1) _mediaInfo->setVideoSpeedInterpolationMode(MediaUtils::BlendFrames);
    else if (index == 2) _mediaInfo->setVideoSpeedInterpolationMode(MediaUtils::MCIO);
    else if (index == 3) _mediaInfo->setVideoSpeedInterpolationMode(MediaUtils::MCIAO);
    else _mediaInfo->setVideoSpeedInterpolationMode(MediaUtils::NoMotionInterpolation);
}

void BlockMotionInterpolation::on_estimationBox_currentIndexChanged(int /*index*/)
{
    if (_freezeUI) return;
    _mediaInfo->setVideoSpeedEstimationMode( estimationBox->currentData().toString() );
}

void BlockMotionInterpolation::on_algorithmBox_currentIndexChanged(int /*index*/)
{
    if (_freezeUI) return;
    _mediaInfo->setVideoSpeedAlgorithm( algorithmBox->currentData().toString() );
}

void BlockMotionInterpolation::on_sceneDetectionBox_clicked(bool checked)
{
    if (_freezeUI) return;
    _mediaInfo->setSceneDetection(checked);
}

void BlockMotionInterpolation::updateInterpolationUI()
{
    int interpoMode = interpolationBox->currentIndex();
    bool interpo = interpoMode == 2 || interpoMode == 3;
    estimationBox->setEnabled(interpo);
    algorithmBox->setEnabled(interpo);
}
