#include "settingswidget.h"

SettingsWidget::SettingsWidget(QSettings *s,QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    settings = s;

    ffmpegPathEdit->setText(settings->value("ffmpeg/path","E:/DEV SRC/DuFFMpeg/ffmpeg/ffmpeg.exe").toString());
}

void SettingsWidget::on_ffmpegPathEdit_textChanged(const QString &arg1)
{
    settings->setValue("ffmpeg/path",ffmpegPathEdit->text());
}
