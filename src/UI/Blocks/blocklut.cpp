#include "blocklut.h"

BlockLut::BlockLut(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
    setType(Type::Video);

    setupUi(this);

    _customLut = FFmpeg::instance()->lut("custom");

    //populate box
    foreach(FFLut *lut, FFmpeg::instance()->luts())
    {
        lutBox->addItem(lut->prettyName(), lut->name());
    }

    inputSpaceBox->setItemData(0, "");
    outputSpaceBox->setItemData(0, "");
    foreach(FFColorProfile *p, FFmpeg::instance()->colorProfiles())
    {
        if (p->name() == "") continue;
        inputSpaceBox->addItem(p->prettyName(), p->name());
        outputSpaceBox->addItem(p->prettyName(), p->name());
    }

    updateLutInputOutputBoxes();

    connect(lutBox, SIGNAL(currentIndexChanged(int)), this, SLOT(lutBox_currentIndexChanged(int)));
    connect(applyBox, SIGNAL(currentIndexChanged(int)), this, SLOT(applyBox_currentIndexChanged(int)));
    connect(inputSpaceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(inputSpaceBox_currentIndexChanged(int)));
    connect(outputSpaceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(outputSpaceBox_currentIndexChanged(int)));
}

void BlockLut::activate(bool blockEnabled)
{
    if (blockEnabled) _mediaInfo->setLut( lutBox->currentData(Qt::UserRole).toString());
    else _mediaInfo->setLut("");
}

void BlockLut::update()
{
    QSignalBlocker b(lutBox);
    QSignalBlocker b2(applyBox);

    VideoInfo *stream =  _mediaInfo->videoStreams().at(0);

    lutBox->setCurrentData(stream->lut()->name());
    updateLutInputOutputBoxes();

    if (stream->applyLutOnOutputSpace()) applyBox->setCurrentIndex(1);
    else applyBox->setCurrentIndex(0);
}

void BlockLut::lutBox_currentIndexChanged(int index)
{
    if (index == lutBox->count() -1 )
    {
#ifdef Q_OS_LINUX
        QString openFileName = QFileDialog::getOpenFileName(this,"Load LUT file", QDir::homePath(), "LUT files (*.3dl *.cube *.dat *.m3d *.csp *.3DL *.CUBE *.DAT *.M3D *.CSP);;After Effects / Autodesk (*.3dl *.3DL);;Iridas (*.cube *.CUBE)::DaVinci (*.dat *.DAT);;Pandora (*.m3d *.M3D);;cineSpace (*.csp *.CSP);;All Files (*.*)");
#else
        QString openFileName = QFileDialog::getOpenFileName(this,"Load LUT file", QDir::homePath(), "LUT files (*.3dl *.cube *.dat *.m3d *.csp);;After Effects / Autodesk (*.3dl);;Iridas (*.cube)::DaVinci (*.dat);;Pandora (*.m3d);;cineSpace (*.csp);;All Files (*.*)");
#endif
        lutBox->setItemData(index, openFileName);
        _customLut->setName(openFileName);

        if (openFileName == "")
        {
            lutBox->setCurrentIndex(0);
            _customLut->setName("custom");
        }
    }

    _mediaInfo->setLut(lutBox->currentData(Qt::UserRole).toString());

    updateLutInputOutputBoxes();
}

void BlockLut::applyBox_currentIndexChanged(int index)
{
    _mediaInfo->setApplyLutOnOutputSpace( index == 1 );
}

void BlockLut::inputSpaceBox_currentIndexChanged(int /*index*/)
{
    _customLut->setInputProfile( inputSpaceBox->currentData().toString());
}

void BlockLut::outputSpaceBox_currentIndexChanged(int /*index*/)
{
   _customLut->setOutputProfile( inputSpaceBox->currentData().toString());
}

void BlockLut::updateLutInputOutputBoxes()
{
    bool enabled = lutBox->currentIndex() == lutBox->count() - 1;
    bool none = lutBox->currentIndex() == 0;
    inputSpaceBox->setEnabled(enabled && !none);
    outputSpaceBox->setEnabled(enabled && !none);
    applyBox->setEnabled(!none);

    inputSpaceBox->setCurrentIndex(0);
    outputSpaceBox->setCurrentIndex(0);

    if (!none && !enabled)
    {
        FFLut *lut = FFmpeg::instance()->lut( lutBox->currentData().toString() );
        if (!lut) return;
        foreach(FFColorProfile *p, FFmpeg::instance()->colorProfiles())
        {
            if (p->name() == lut->inputProfile()) inputSpaceBox->setCurrentData(p->name());
            if (p->name() == lut->outputProfile()) outputSpaceBox->setCurrentData(p->name());
        }
    }
}
