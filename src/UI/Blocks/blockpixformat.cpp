#include "blockpixformat.h"

BlockPixFormat::BlockPixFormat(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    setupUi(this);
    _freezeUI = true;
    listPixFormats(true);
    _freezeUI = false;
}

void BlockPixFormat::setActivated(bool activate)
{
    if (activate)
    {
        _mediaInfo->setPixFormat( pixFmtBox->currentData(Qt::UserRole).toString() );
    }
    else
    {
        _mediaInfo->setPixFormat( nullptr );
    }
}

void BlockPixFormat::update()
{
    qDebug() << "updated";
    listPixFormats( false );

    FFPixFormat *pf = _mediaInfo->pixFormat();
    if ( pf == nullptr )
    {
        setDefaultPixFormat();
    }
    else
    {
        setPixFormat( pf->name() );
    }
}

void BlockPixFormat::listPixFormats( bool init )
{
    _freezeUI = true;
    pixFmtBox->clear();
    QString prevFilter = pixFmtFilterBox->currentText();

    FFCodec *vc = _mediaInfo->videoCodec();
    if ( vc == nullptr ) vc = _mediaInfo->defaultVideoCodec();
    if ( vc == nullptr )
    {
        _freezeUI = false;
        return;
    }

    bool alpha = _mediaInfo->hasAlpha();
    QStringList filters = QStringList();
    foreach( FFPixFormat *p, vc->pixFormats() )
    {
        QString filter = QString::number(p->bitsPerPixel()) + "bits (" + QString::number(p->numComponents()) + " channels @" + QString::number(p->bitsPerComponent()) + "bpc)" ;
        if ( p->hasAlpha() == alpha && ( p->prettyName().indexOf(prevFilter) > 0 || pixFmtFilterBox->currentIndex() == 0  || init ) )
        {
            pixFmtBox->addItem(p->prettyName(), p->name() );
            if (!filters.contains(filter)) filters << filter;
        }
    }

    if (init)
    {
        filters.sort();
        pixFmtFilterBox->clear();
        pixFmtFilterBox->addItem("All bit depths",0);
        foreach(QString filter,filters)
        {
            pixFmtFilterBox->addItem(filter);
        }
    }
    _freezeUI = false;
}

void BlockPixFormat::setDefaultPixFormat()
{
    _freezeUI = true;
    FFCodec *vc = _mediaInfo->videoCodec();
    if ( vc == nullptr ) vc = _mediaInfo->defaultVideoCodec();
    if ( vc == nullptr ) return;

    FFPixFormat *pf = vc->defaultPixFormat();

    if (pf != nullptr ) setPixFormat( pf->name() );
    _freezeUI = false;
}

void BlockPixFormat::setPixFormat(QString name)
{
    _freezeUI = true;
    if ( name == "" ) setDefaultPixFormat();

    for(int i = 0; i < pixFmtBox->count(); i++)
    {
        if (pixFmtBox->itemData(i, Qt::UserRole).toString() == name )
        {
            pixFmtBox->setCurrentIndex( i );
            return;
        }
    }

    //try again without filter
    listPixFormats( true );
    for(int i = 0; i < pixFmtBox->count(); i++)
    {
        if (pixFmtBox->itemData(i, Qt::UserRole).toString() == name )
        {
            pixFmtBox->setCurrentIndex( i );
            return;
        }
    }
    _freezeUI = false;
}

void BlockPixFormat::on_pixFmtFilterBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    listPixFormats( false );
}

void BlockPixFormat::on_pixFmtBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    _mediaInfo->setPixFormat( pixFmtBox->itemData(index, Qt::UserRole).toString() );
}
