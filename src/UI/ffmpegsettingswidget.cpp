#include "ffmpegsettingswidget.h"

FFmpegSettingsWidget::FFmpegSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _freezeUI = true;

    //needed for macos
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    ffmpegPathEdit->setText( QDir::toNativeSeparators( FFmpeg::instance()->binary() ) );
    userPresetsPathEdit->setText(_settings.value("presets/path","").toString());

    connect( FFmpeg::instance(), SIGNAL( statusChanged(MediaUtils::RenderStatus)), this, SLOT ( ffmpegStatus(MediaUtils::RenderStatus)) );

    _freezeUI = false;
}

void FFmpegSettingsWidget::on_ffmpegBrowseButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select the ffmpeg executable binary");
    if (path == "") return;
    ffmpegPathEdit->setText(path);
    QSignalBlocker b(ffmpegPathEdit);
    FFmpeg::instance()->setBinary( path, true );
}

void FFmpegSettingsWidget::on_ffmpegPathEdit_editingFinished()
{
    FFmpeg::instance()->setBinary( ffmpegPathEdit->text(), true );
}

void FFmpegSettingsWidget::on_userPresetsBrowseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Select the folder containing user presets",_settings.value("presets/path").toString());
    if (path == "") return;
    userPresetsPathEdit->setText(path);
    _settings.setValue("presets/path",path);
    _settings.sync();
    emit presetsPathChanged();
}

void FFmpegSettingsWidget::on_userPresetsPathEdit_editingFinished()
{
    _settings.setValue("presets/path",userPresetsPathEdit->text());
    _settings.sync();
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


void FFmpegSettingsWidget::on_openButton_clicked()
{
    FileUtils::openInExplorer(PresetManager::instance()->userPresetsPath());
}
