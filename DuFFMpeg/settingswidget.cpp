#include "settingswidget.h"

SettingsWidget::SettingsWidget(QSettings *s,QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    settings = s;

    ffmpegPathEdit->setText(settings->value("ffmpeg/path","E:/DEV SRC/DuFFMpeg/ffmpeg/ffmpeg.exe").toString());
}

void SettingsWidget::changeFFmpegPath(QString path)
{
    settings->setValue("ffmpeg/path",ffmpegPathEdit->text());
    emit ffmpegPathChanged(path);
}

void SettingsWidget::on_ffmpegPathEdit_textChanged(const QString &arg1)
{
    changeFFmpegPath(arg1);
}

void SettingsWidget::on_ffmpegBrowseButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select the ffmpeg executable binary");
    if (path == "") return;
    ffmpegPathEdit->setText(path);
    changeFFmpegPath(path);
}
