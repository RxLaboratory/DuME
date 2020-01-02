#include "blockpixformat.h"

BlockPixFormat::BlockPixFormat(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    setupUi(this);
    _freezeUI = true;
    listPixFormats();
    _freezeUI = false;
}

void BlockPixFormat::setActivated(bool activate)
{
    if (activate && pixFmtFilterBox->currentIndex() != 0 )
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
    listPixFormats( );

    _freezeUI = true;

    filterPixFormats();

    _freezeUI = true;

    FFPixFormat *pf = _mediaInfo->pixFormat();
    if ( pf == nullptr )
    {
        setDefaultPixFormat();
    }
    else
    {
        setPixFormat( pf->name() );
    }

    _freezeUI = false;
}

void BlockPixFormat::listPixFormats()
{
    _freezeUI = true;

    QString prevFilter = pixFmtFilterBox->currentText();
    QString prevFormat = pixFmtBox->currentData().toString();
    pixFmtBox->clear();
    pixFmtFilterBox->clear();
    _pixFormats.clear();

    FFCodec *vc = _mediaInfo->videoCodec();
    if ( vc == nullptr ) vc = _mediaInfo->defaultVideoCodec();
    if ( vc == nullptr )
    {
        _freezeUI = false;
        return;
    }

    _pixFormats = vc->pixFormats();

    QStringList filters = QStringList();
    foreach( FFPixFormat *p, vc->pixFormats() )
    {
        QString filter = QString::number(p->bitsPerPixel()) + " bits - " + QString::number(p->numComponents()) + " channels" ;
        if (!filters.contains(filter)) filters << filter;
    }

    filters.sort();
    pixFmtFilterBox->addItem("Default");
    pixFmtFilterBox->addItem("All bit depths");
    foreach(QString filter,filters)
    {
        pixFmtFilterBox->addItem(filter);
    }

    // reselect previous
    setFilter( prevFilter );
    setPixFormat( prevFormat );

    _freezeUI = false;
}

void BlockPixFormat::filterPixFormats()
{
    _freezeUI = true;

    QString prevFormat = pixFmtBox->currentData().toString();
    pixFmtBox->clear();

    bool alpha = _mediaInfo->hasAlpha();
    QString filter = pixFmtFilterBox->currentText();
    foreach( FFPixFormat *p, _pixFormats )
    {
        if ( p->hasAlpha() == alpha && ( p->prettyName().indexOf( filter ) > 0 || pixFmtFilterBox->currentIndex() <= 1 ) )
        {
            pixFmtBox->addItem(p->prettyName(), p->name() );
        }
    }

    setPixFormat( prevFormat, false );

    _freezeUI = false;
}

void BlockPixFormat::setDefaultPixFormat()
{
    _freezeUI = true;

    pixFmtFilterBox->setCurrentIndex( 0 );
    filterPixFormats();

    _freezeUI = true;

    FFPixFormat *pf = _mediaInfo->defaultPixFormat();

    if (pf == nullptr)
    {
        pixFmtBox->setCurrentIndex( -1 );
    }
    else
    {
        setPixFormat( pf->name() );
    }

    pixFmtBox->setEnabled( false );

    _freezeUI = false;
}

void BlockPixFormat::setPixFormat(QString name, bool tryWithoutFilter )
{
    _freezeUI = true;

    for(int i = 0; i < pixFmtBox->count(); i++)
    {
        if (pixFmtBox->itemData( i ).toString() == name )
        {
            pixFmtBox->setCurrentIndex( i );
            _freezeUI = false;
            return;
        }
    }

    //try again without filter
    if ( tryWithoutFilter )
    {
        pixFmtFilterBox->setCurrentIndex( 0 );
        filterPixFormats();
        setPixFormat( name, false );
    }
    else
    {
        pixFmtBox->setCurrentIndex( -1 );
    }

    _freezeUI = false;
}

void BlockPixFormat::setFilter(QString name)
{
    _freezeUI = true;

    pixFmtFilterBox->setCurrentText( name );
    filterPixFormats();

    _freezeUI = false;
}

void BlockPixFormat::on_pixFmtFilterBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    if ( index == 0 )
    {
        _mediaInfo->setPixFormat( nullptr );
    }
    else
    {
        filterPixFormats();
        pixFmtBox->setEnabled(true);
    }

}

void BlockPixFormat::on_pixFmtBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    _mediaInfo->setPixFormat( pixFmtBox->itemData(index, Qt::UserRole).toString() );
}
