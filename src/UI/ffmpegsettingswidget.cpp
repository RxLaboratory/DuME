#include "ffmpegsettingswidget.h"

FFmpegSettingsWidget::FFmpegSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _freezeUI = true;

    ffmpegPathEdit->setText( QDir::toNativeSeparators( ffmpeg->binary() ) );
    userPresetsPathEdit->setText(settings.value("presets/path","").toString());

    connect( ffmpeg, SIGNAL( statusChanged(MediaUtils::RenderStatus)), this, SLOT ( ffmpegStatus(MediaUtils::RenderStatus)) );

    _freezeUI = false;
}

void FFmpegSettingsWidget::on_ffmpegBrowseButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select the ffmpeg executable binary");
    if (path == "") return;
    ffmpegPathEdit->setText(path);
    ffmpeg->setBinary( path, true );
}

void FFmpegSettingsWidget::on_ffmpegPathEdit_editingFinished()
{
    ffmpeg->setBinary( ffmpegPathEdit->text(), true );
}

void FFmpegSettingsWidget::on_userPresetsBrowseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Select the folder containing user presets",settings.value("presets/path").toString());
    if (path == "") return;
    userPresetsPathEdit->setText(path);
    settings.setValue("presets/path",path);
    settings.sync();
    emit presetsPathChanged();
}

void FFmpegSettingsWidget::on_userPresetsPathEdit_editingFinished()
{
    settings.setValue("presets/path",userPresetsPathEdit->text());
    settings.sync();
    emit presetsPathChanged();
}

void FFmpegSettingsWidget::ffmpegStatus(MediaUtils::RenderStatus status)
{
    if ( MediaUtils::isBusy(status) )
    {
        pathWidget->setEnabled(false);
        ffmpegBrowseButton->setText("Busy");
    }
    else
    {
        pathWidget->setEnabled(true);
        ffmpegBrowseButton->setText("Browse...");
    }
}

