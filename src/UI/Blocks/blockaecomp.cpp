#include "blockaecomp.h"

BlockAEComp::BlockAEComp(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
    setupUi(this);
}

void BlockAEComp::activate(bool activate)
{
    _freezeUI = true;
    if(activate)
    {
        _mediaInfo->setAeUseRQueue( aeRenderQueueButton->isChecked() );
        if ( compButton->isChecked() ) _mediaInfo->setAepCompName( compEdit->text());
        else _mediaInfo->setAepCompName( "" );
        if ( rqindexButton->isChecked() ) _mediaInfo->setAepRqindex( rqindexBox->value() );
        else _mediaInfo->setAepRqindex( -1 );
    }
    else
    {
        _mediaInfo->setAeUseRQueue( true );
        _mediaInfo->setAepCompName( "" );
        _mediaInfo->setAepRqindex( -1 );
    }

    _freezeUI = false;
}

void BlockAEComp::update()
{
    _freezeUI = true;

    if (_mediaInfo->aeUseRQueue())
    {
        aeRenderQueueButton->setChecked( true );
        compEdit->setEnabled( false );
        rqindexBox->setEnabled( false );
    }
    else if ( _mediaInfo->aepCompName() != "")
    {
        compButton->setChecked( true );
        compEdit->setEnabled( true );
        rqindexBox->setEnabled( false );
        compEdit->setText( _mediaInfo->aepCompName() );
    }
    else if ( _mediaInfo->aepRqindex() > 0 )
    {
        rqindexButton->setChecked( true );
        compEdit->setEnabled( false );
        rqindexBox->setEnabled( true );
        rqindexBox->setValue( _mediaInfo->aepRqindex() );

    }

    _freezeUI = false;
}

void BlockAEComp::on_compButton_clicked(bool checked)
{
    compEdit->setEnabled(checked);
    rqindexBox->setEnabled(!checked);
    if ( _freezeUI ) return;
    if (checked)
    {
        QString comp = compEdit->text();
        if (comp == "") comp = "Name";
        _mediaInfo->setAepCompName( comp );
        _mediaInfo->setAepRqindex( -1 );
    }
}

void BlockAEComp::on_rqindexButton_clicked(bool checked)
{
    compEdit->setEnabled(!checked);
    rqindexBox->setEnabled(checked);
    if ( _freezeUI ) return;
    if (checked)
    {
        _mediaInfo->setAepCompName( "" );
        _mediaInfo->setAepRqindex( rqindexBox->value() );
    }
}

void BlockAEComp::on_aeRenderQueueButton_clicked(bool checked)
{
    compEdit->setEnabled(!checked);
    rqindexBox->setEnabled(!checked);
    if ( _freezeUI ) return;
    _mediaInfo->setAeUseRQueue( checked );
    if (checked)
    {
        _mediaInfo->setAepCompName( "" );
        _mediaInfo->setAepRqindex( -1 );
    }
}

void BlockAEComp::on_compEdit_editingFinished()
{
    if ( _freezeUI ) return;
    _mediaInfo->setAepCompName( compEdit->text() );
}

void BlockAEComp::on_rqindexBox_valueChanged(int arg1)
{
    if ( _freezeUI ) return;
    _mediaInfo->setAepRqindex( arg1 );
}
