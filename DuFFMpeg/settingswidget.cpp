#include "settingswidget.h"

SettingsWidget::SettingsWidget(QSettings *s,QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    settings = s;

    ffmpegPathEdit->setText(settings->value("ffmpeg/path","ffmpeg.exe").toString());
    userPresetsPathEdit->setText(settings->value("presets/path","").toString());
}

void SettingsWidget::on_ffmpegBrowseButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select the ffmpeg executable binary");
    if (path == "") return;
    if (settings->value("ffmpeg/path").toString() ==  path) return;
    ffmpegPathEdit->setText(path);
    settings->setValue("ffmpeg/path",ffmpegPathEdit->text());
    emit ffmpegPathChanged(path);
}

void SettingsWidget::on_ffmpegPathEdit_editingFinished()
{
    if (settings->value("ffmpeg/path").toString() == ffmpegPathEdit->text()) return;
    settings->setValue("ffmpeg/path",ffmpegPathEdit->text());
    emit ffmpegPathChanged(ffmpegPathEdit->text());
}

void SettingsWidget::on_userPresetsBrowseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Select the folder containing user presets",settings->value("presets/path").toString());
    if (path == "") return;
    userPresetsPathEdit->setText(path);
    settings->setValue("presets/path",path);
    emit presetsPathChanged(path);
}

void SettingsWidget::on_userPresetsPathEdit_editingFinished()
{
    settings->setValue("presets/path",userPresetsPathEdit->text());
    emit presetsPathChanged(userPresetsPathEdit->text());
}
