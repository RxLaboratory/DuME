#include "settingswidget.h"
#include "dropshadow.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

SettingsWidget::SettingsWidget(AfterEffects *ae, QWidget *parent) :
    QWidget(parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Settings Widget";
#endif
    setupUi(this);

    _ae = ae;
    _freezeUI = true;

    //Add nice shadows
    generalWidget->setGraphicsEffect(new DropShadow);
    aeWidget->setGraphicsEffect(new DropShadow);

    ffmpegPathEdit->setText( QDir::toNativeSeparators( ffmpeg->binary() ) );
    userPresetsPathEdit->setText(settings.value("presets/path","").toString());

    //Temp path
    QString cachePath = QDir::toNativeSeparators( cacheManager->getRootCacheDir().absolutePath() );
    aeCacheEdit->setText(cachePath);

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

    _freezeUI = false;
}

void SettingsWidget::on_ffmpegBrowseButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select the ffmpeg executable binary");
    if (path == "") return;
    ffmpegPathEdit->setText(path);
    ffmpeg->setBinary( path, true );
}

void SettingsWidget::on_ffmpegPathEdit_editingFinished()
{
    ffmpeg->setBinary( ffmpegPathEdit->text(), true );
}

void SettingsWidget::on_userPresetsBrowseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Select the folder containing user presets",settings.value("presets/path").toString());
    if (path == "") return;
    userPresetsPathEdit->setText(path);
    settings.setValue("presets/path",path);
    settings.sync();
    emit presetsPathChanged();
}

void SettingsWidget::on_userPresetsPathEdit_editingFinished()
{
    settings.setValue("presets/path",userPresetsPathEdit->text());
    settings.sync();
    emit presetsPathChanged();
}

void SettingsWidget::on_aeVersionBox_currentIndexChanged(int index)
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

void SettingsWidget::on_aerenderPathEdit_textChanged(const QString &arg1)
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

void SettingsWidget::on_aerenderBrowseButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select the aerender executable binary",settings.value("aerender/path","").toString());
    if (path == "") return;
    aerenderPathEdit->setText(path);
}

void SettingsWidget::on_aeCacheEdit_textChanged(const QString &arg1)
{
    if (_freezeUI) return;
    _freezeUI = true;
    QString path = arg1;
    if (!path.endsWith("/") || !path.endsWith("\\")) path = path + "/";
    path = QDir::toNativeSeparators(path);

    if (QDir(arg1).exists())
    {
        cacheManager->setRootCacheDir(path);
        aeCacheEdit->setText(path);
    }
    _freezeUI = false;
}

void SettingsWidget::on_aeCacheBrowseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Select the aerender cache directory",cacheManager->getRootCacheDir().absolutePath());
    if (path == "") return;
    aeCacheEdit->setText(path);
}

void SettingsWidget::refreshAeVersionBox()
{
    _freezeUI = true;

    foreach(AfterEffectsVersion *ae, _ae->versions())
    {
        aeVersionBox->addItem(ae->name(), ae->path());
    }
    aeVersionBox->addItem("Latest", "Latest" );
    aeVersionBox->addItem("Custom", _ae->binary() );

    _freezeUI = false;
}

void SettingsWidget::on_resetDefaultsButton_clicked()
{
    settings.clear();
}
