#include "blockcustom.h"

BlockCustom::BlockCustom(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
    setupUi(this);
    _freezeUI = true;
    _presets->addAction( actionAddParameter );
    addParam();
    _freezeUI = false;
}

void BlockCustom::activate(bool blockEnabled)
{
    if (blockEnabled)
    {
        _freezeUI = false;
        updateParams();
    }
    else
    {
        _mediaInfo->clearFFmpegOptions();
    }
}

void BlockCustom::update()
{
    removeParams();
    foreach(QStringList o, _mediaInfo->ffmpegOptions())
    {
        addParam(o[0], o[1]);
    }
    //keep one empty
    if (_paramWidgets.count() == 0)
    {
        addParam();
    }
}

void BlockCustom::updateParams()
{
    if (_freezeUI) return;
    bool frozen = _freezeUI;
    _freezeUI = true;
    _mediaInfo->clearFFmpegOptions();
    foreach(FFmpegParamWidget *w, _paramWidgets)
    {
        QString p = w->param();
        if (p != "")
        {
            QStringList o(p);
            o << w->value();
            _mediaInfo->addFFmpegOption( o );
        }
    }
    // remove empty params
    for( int i = _paramWidgets.count() -1; i >= 0; i--)
    {
        FFmpegParamWidget *w = _paramWidgets[i];
        if(w->param() == "" && w->value() == "")
        {
            _paramWidgets.removeAt(i);
            w->hide();
            w->deleteLater();
        }
    }
    //keep one empty
    if (_paramWidgets.count() == 0)
    {
        addParam();
    }
    _freezeUI = frozen;
}

void BlockCustom::addParam()
{
    bool frozen = _freezeUI;
    _freezeUI = true;
    FFmpegParamWidget *w = new FFmpegParamWidget();
    mainLayout->addWidget(w);
    connect(w, &FFmpegParamWidget::changed, this, &BlockCustom::updateParams);
    _paramWidgets << w;
    _freezeUI = frozen;
}

void BlockCustom::addParam(QString p, QString v)
{
    bool frozen = _freezeUI;
    _freezeUI = true;
    addParam();
    FFmpegParamWidget *w = _paramWidgets.last();
    w->setParam(p);
    w->setValue(v);
    _freezeUI = frozen;
}

void BlockCustom::removeParams()
{
    bool frozen = _freezeUI;
    _freezeUI = true;

    while(_paramWidgets.count() > 0)
    {
        _paramWidgets.takeFirst()->deleteLater();
    }

    _freezeUI = frozen;
}

void BlockCustom::on_actionAddParameter_triggered()
{
    addParam();
}
