#include "blocklut.h"

BlockLut::BlockLut(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
    setType(Type::Video);

    setupUi(this);

    //populate box
    lutBox->addItem("None", "");
    lutBox->addItem("Adobe Gamma 2.4 to QT Gamma 1.96", ":/luts/adobe-to-qt.cube");
    lutBox->addItem("QT Gamma 1.96 to Adobe Gamma 2.4", ":/luts/qt-to-adobe.cube");
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
        QString openFileName = QFileDialog::getOpenFileName(this,"Load LUT file", QDir::homePath(), "LUT files (*.3dl *.cube *.dat *.m3d *.csp);;After Effects (*.3dl);;Iridas (*.cube)::DaVinci (*.dat);;Pandora (*.m3d);;cineSpace (*.csp);;All Files (*.*)");
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
}

void BlockLut::applyBox_currentIndexChanged(int index)
{
    _mediaInfo->setApplyLutOnOutputSpace( index == 1 );
}
