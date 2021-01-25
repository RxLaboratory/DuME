#include "blockpixformat.h"

BlockPixFormat::BlockPixFormat(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create PixFormat block";
#endif
    setType(Type::Video);
    setupUi(this);
    listPixFormats();
}

void BlockPixFormat::activate(bool activate)
{
    if (activate && pixFmtFilterBox->currentIndex() != 0 )
    {
        _mediaInfo->setPixFormat( pixFmtBox->currentData(Qt::UserRole).toString() );
    }
    else
    {
        _mediaInfo->setPixFormat( "" );
    }
}

void BlockPixFormat::update()
{
    VideoInfo *stream = _mediaInfo->videoStreams()[0];

    listPixFormats( );

    if (_pixFormats.count() == 0)
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    filterPixFormats(false);

    FFPixFormat *pf = stream->pixFormat();
    setPixFormat( pf->name() );
}

void BlockPixFormat::listPixFormats()
{
    bool freeze = _freezeUI;
    _freezeUI = true;

    QString prevFilter = pixFmtFilterBox->currentText();
    QString prevFormat = pixFmtBox->currentData().toString();
    pixFmtBox->clear();
    pixFmtFilterBox->clear();
    _pixFormats.clear();

    if (!_mediaInfo->hasVideo())
    {
        _freezeUI = false;
        return;
    }

    FFCodec *vc = _mediaInfo->videoStreams()[0]->codec();
    if ( vc->name() == "" ) vc = _mediaInfo->defaultVideoCodec();

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

    if (_pixFormats.count() == 0)
    {
        _freezeUI = freeze;
        return;
    }

    // reselect previous
    setFilter( prevFilter );
    setPixFormat( prevFormat );

    _freezeUI = freeze;
}

void BlockPixFormat::filterPixFormats(bool resetPrevious)
{
    bool freeze = _freezeUI;
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

    if (resetPrevious) setPixFormat( prevFormat, false );

    _freezeUI = freeze;
}

void BlockPixFormat::setDefaultPixFormat()
{
    bool freeze = _freezeUI;
    _freezeUI = true;

    pixFmtFilterBox->setCurrentIndex( 0 );
    filterPixFormats(false);

    if (!_mediaInfo->hasVideo())
    {
        _freezeUI = freeze;
        return;
    }

    FFPixFormat *pf = _mediaInfo->videoStreams()[0]->defaultPixFormat();
    if (pf->name() == "") pf = _mediaInfo->defaultPixFormat();

    if (pf->name() == "")
    {
        pixFmtBox->setCurrentIndex( -1 );
    }
    else
    {
        setPixFormat( pf->name(), false );
    }

    pixFmtBox->setEnabled( false );

    _freezeUI = freeze;
}

void BlockPixFormat::setPixFormat(QString name, bool tryWithoutFilter )
{
    bool freeze = _freezeUI;
    _freezeUI = true;

    if (name == "" && _pixFormats.count() > 0)
    {
        setDefaultPixFormat();
        _freezeUI = freeze;
        return;
    }

    pixFmtBox->setCurrentData(name);
    if (pixFmtBox->currentIndex() >= 0)
    {
        _freezeUI = freeze;
        return;
    }

    //try again without filter
    if ( tryWithoutFilter )
    {
        pixFmtFilterBox->setCurrentIndex( 1 );
        filterPixFormats();
        setPixFormat( name, false );
    }
    else
    {
        pixFmtBox->setCurrentIndex( -1 );
    }

    _freezeUI = freeze;
}

void BlockPixFormat::setFilter(QString name)
{
    bool freeze = _freezeUI;
    _freezeUI = true;

    pixFmtFilterBox->setCurrentText( name );
    filterPixFormats();

    _freezeUI = freeze;
}

void BlockPixFormat::on_pixFmtFilterBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    if ( index == 0 )
    {
        _mediaInfo->setPixFormat( "" );
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
