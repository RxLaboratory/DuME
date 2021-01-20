#include "blockcrop.h"

BlockCrop::BlockCrop(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Crop block";
#endif
    setType(Type::Video);

    _freezeUI = true;

    setupUi(this);

    _presets->addAction( actionSmallSquare );
    _presets->addAction( actionHDSquare );
    _presets->addAction( actionHD720 );
    _presets->addAction( actionHD1080 );
    _presets->addAction( action2KFlat );
    _presets->addAction( action2KScope );
    _presets->addAction( actionDSM2K );
    _presets->addAction( action4KUHD );
    _presets->addAction( action4KFlat );
    _presets->addAction( action4KScope );
    _presets->addAction( actionDSM4K );
    _presets->addAction( action8KUHD );

    connect(top, SIGNAL(valueChanged(int)), this, SLOT(setCropMargins()));
    connect(bottom, SIGNAL(valueChanged(int)), this, SLOT(setCropMargins()));
    connect(left, SIGNAL(valueChanged(int)), this, SLOT(setCropMargins()));
    connect(right, SIGNAL(valueChanged(int)), this, SLOT(setCropMargins()));
    connect(widthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setCropSize()));
    connect(heightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setCropSize()));

    _freezeUI = false;
}

void BlockCrop::activate(bool blockEnabled)
{
    bool freeze = _freezeUI;
    _freezeUI = true;

    if (blockEnabled)
    {
        _mediaInfo->setCrop(top->value(), bottom->value(), left->value(), right->value());
        _mediaInfo->setCrop(widthSpinBox->value(), heightSpinBox->value());
        _mediaInfo->setCropUseSize(!setMarginsButton->isChecked());
    }
    else
    {
        _mediaInfo->setCrop(0,0,0,0);
        _mediaInfo->setCrop(0,0);
        _mediaInfo->setCropUseSize(false);
    }

    _freezeUI = freeze;
}

void BlockCrop::update()
{
    if (_freezeUI) return;
    bool freeze = _freezeUI;
    _freezeUI = true;

    VideoInfo *stream = _mediaInfo->videoStreams()[0];
    top->setValue(stream->topCrop());
    bottom->setValue(stream->bottomCrop());
    left->setValue(stream->leftCrop());
    right->setValue(stream->rightCrop());
    widthSpinBox->setValue(stream->cropWidth());
    heightSpinBox->setValue(stream->cropHeight());
    setMarginsButton->setChecked(!stream->cropUseSize());
    setCropSizeButton->setChecked(stream->cropUseSize());

    _freezeUI = freeze;
}

void BlockCrop::setMaximum(int w, int h)
{
    top->setMaximum(h);
    bottom->setMaximum(h);
    left->setMaximum(w);
    right->setMaximum(w);
    widthSpinBox->setMaximum(w);
    heightSpinBox->setMaximum(h);
}

void BlockCrop::setCropMargins()
{
    if (_freezeUI) return;
    _mediaInfo->setCrop(
                top->value(),
                bottom->value(),
                left->value(),
                right->value()
                );
}

void BlockCrop::setCropSize()
{
    if (_freezeUI) return;
    _mediaInfo->setCrop(
                widthSpinBox->value(),
                heightSpinBox->value()
                );
}

void BlockCrop::on_setMarginsButton_toggled(bool checked)
{
    top->setEnabled(checked);
    bottom->setEnabled(checked);
    left->setEnabled(checked);
    right->setEnabled(checked);
    widthSpinBox->setEnabled(!checked);
    heightSpinBox->setEnabled(!checked);
    if (_freezeUI) return;
    _mediaInfo->setCropUseSize(!checked);
}

void BlockCrop::on_widthSpinBox_valueChanged(int arg1)
{
    if (arg1 == 0) widthSpinBox->setSuffix(" Same as source");
    else widthSpinBox->setSuffix("px");
}

void BlockCrop::on_heightSpinBox_valueChanged(int arg1)
{
    if (arg1 == 0) heightSpinBox->setSuffix(" Same as source");
    else heightSpinBox->setSuffix("px");
}

void BlockCrop::setSize(int width, int height)
{
    setCropSizeButton->setChecked(true);
    setMarginsButton->setChecked(false);
    widthSpinBox->setValue(width);
    heightSpinBox->setValue(height);
}

void BlockCrop::on_actionHD1080_triggered()
{
    setSize( 1920, 1080 );
}

void BlockCrop::on_action2KFlat_triggered()
{
    setSize( 1998, 1080 );
}

void BlockCrop::on_action2KScope_triggered()
{
    setSize( 2048, 858 );
}

void BlockCrop::on_actionDSM2K_triggered()
{
    setSize( 2048, 1080 );
}

void BlockCrop::on_action4KFlat_triggered()
{
    setSize( 3996, 2160 );
}

void BlockCrop::on_action4KScope_triggered()
{
    setSize( 4096, 1716 );
}

void BlockCrop::on_actionDSM4K_triggered()
{
    setSize( 4096, 2160 );
}

void BlockCrop::on_action4KUHD_triggered()
{
    setSize( 3840, 2160 );
}

void BlockCrop::on_action8KUHD_triggered()
{
    setSize( 7680, 4320 );
}

void BlockCrop::on_actionHDSquare_triggered()
{
    setSize(1080,1080);
}

void BlockCrop::on_actionSmallSquare_triggered()
{
    setSize(720,720);
}
