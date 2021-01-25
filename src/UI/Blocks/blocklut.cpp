#include "blocklut.h"

BlockLut::BlockLut(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create LUT block";
#endif
    setType(Type::Video);

    _freezeUI = true;
    setupUi(this);

    //populate box
    lutBox->addItem("None", "");
    lutBox->addItem("Adobe Gamma 2.4 to QT Gamma 1.96", ":/luts/adobe-to-qt.cube");
    lutBox->addItem("QT Gamma 1.96 to Adobe Gamma 2.4", ":/luts/qt-to-adobe.cube");
    lutBox->addItem("Custom...","");

    _freezeUI = false;
}

void BlockLut::activate(bool blockEnabled)
{
    if (blockEnabled) _mediaInfo->setLut( lutBox->currentData(Qt::UserRole).toString());
    else _mediaInfo->setLut("");
}

void BlockLut::update()
{
    VideoInfo *stream = _mediaInfo->videoStreams()[0];
    lutBox->setCurrentData(stream->lut());
}

void BlockLut::on_lutBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
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
