#include "aesettingswidget.h"

AESettingsWidget::AESettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _ae = AfterEffects::instance();
    _freezeUI = true;

    updateAe();

    connect(_ae, &AbstractRendererInfo::binaryChanged, this, &AESettingsWidget::updateAe);

    _freezeUI = false;
}

void AESettingsWidget::on_aeVersionBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    _freezeUI = true;

    QString name = aeVersionBox->itemText(index);
    _ae->setBinary( name );

    aerenderPathEdit->setText( QDir::toNativeSeparators( _ae->binary() ) );
    aerenderBrowseButton->setEnabled( name == "Custom" );
    aerenderPathEdit->setEnabled( name == "Custom" );

    _freezeUI = false;
}

void AESettingsWidget::on_aerenderPathEdit_textChanged(const QString &arg1)
{
    if (_freezeUI) return;
    _freezeUI = true;

    QString path = QDir::toNativeSeparators(arg1);
    aerenderPathEdit->setText(path);

    for( int i = 0; i < aeVersionBox->count(); i++)
    {
        if (aeVersionBox->itemData(i).toString() == path )
        {
            _freezeUI = false;
            aeVersionBox->setCurrentIndex(i);
            return;
        }
    }

    settings.setValue( "aerender/path", path );
    settings.sync();

    _freezeUI = false;
    aeVersionBox->setCurrentText( "Custom" );
}

void AESettingsWidget::on_aerenderBrowseButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select the aerender executable binary",settings.value("aerender/path","").toString());
    if (path == "") return;
    aerenderPathEdit->setText(path);
}

void AESettingsWidget::updateAe()
{
    bool frozen = _freezeUI;
    _freezeUI = true;;

    //aerender path
    QString aerenderPath = _ae->binary();
    if (aerenderPath == "") aerenderPath = "Not Found! You may need to install Adobe After Effects.";
    aerenderPath = QDir::toNativeSeparators(aerenderPath);
    aerenderPathEdit->setText(aerenderPath);
    //ae versions
    refreshAeVersionBox();

    //select the right item
    if ( _ae->useLatest() ) aeVersionBox->setCurrentText( "Latest" );
    else aeVersionBox->setCurrentText( _ae->currentName() );

    aerenderPathEdit->setEnabled( _ae->currentName() == "Custom" );
    aerenderBrowseButton->setEnabled( _ae->currentName() == "Custom" );

    _freezeUI = frozen;
}

void AESettingsWidget::refreshAeVersionBox()
{
    bool frozen = _freezeUI;
    _freezeUI = true;

    aeVersionBox->clear();

    foreach(AfterEffectsVersion *ae, _ae->versions())
    {
        aeVersionBox->addItem(ae->name(), ae->path());
    }
    aeVersionBox->addItem("Latest", "Latest" );
    aeVersionBox->addItem("Custom", _ae->binary() );

    _freezeUI = frozen;
}
