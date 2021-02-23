#include "blocklut.h"

BlockLut::BlockLut(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
    setType(Type::Video);

    setupUi(this);

    //populate box
    foreach(FFLut *lut, FFmpeg::instance()->luts())
    {
        lutBox->addItem(lut->prettyName(), lut->name());
    }
    lutBox->addItem("Custom...","");

    connect(lutBox, SIGNAL(currentIndexChanged(int)), this, SLOT(lutBox_currentIndexChanged(int)));
    connect(applyBox, SIGNAL(currentIndexChanged(int)), this, SLOT(applyBox_currentIndexChanged(int)));
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

    lutBox->setCurrentData(stream->lut());

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
        if (openFileName == "")
        {
            lutBox->setCurrentIndex(0);
        }
    }
    else if (index > 0 && index < lutBox->count() -1)
    {
        //extract LUT
        CacheManager *cm = CacheManager::instance();
        QDir d = cm->getRootCacheDir();
        QString sourceLutPath = lutBox->currentData(Qt::UserRole).toString();
        QFileInfo sourceInfo(sourceLutPath);
        QString destinationLutPath = d.absolutePath() + "/" + sourceInfo.fileName();
        QFile::copy(sourceLutPath, destinationLutPath);
        lutBox->setItemData(index, destinationLutPath);
    }
    _mediaInfo->setLut(lutBox->currentData(Qt::UserRole).toString());

    applyBox->setEnabled(index != 0);
}

void BlockLut::applyBox_currentIndexChanged(int index)
{
    _mediaInfo->setApplyLutOnOutputSpace( index == 1 );
}
