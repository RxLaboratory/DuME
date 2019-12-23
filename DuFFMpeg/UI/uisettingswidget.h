#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_settingswidget.h"

#include <QSettings>
#include <QFileDialog>
#include <QDir>
#include <QTemporaryDir>
#include <QFile>

#include "ffmpeg.h"

class UISettingsWidget : public QWidget, private Ui::SettingsWidget
{
    Q_OBJECT

public:
    explicit UISettingsWidget(FFmpeg *ffmpeg, QWidget *parent = nullptr);

signals:
    void ffmpegPathChanged(QString);
    void presetsPathChanged(QString);

private slots:
    void on_ffmpegBrowseButton_clicked();
    void on_ffmpegPathEdit_editingFinished();
    void on_userPresetsBrowseButton_clicked();
    void on_userPresetsPathEdit_editingFinished();
    void on_aeVersionBox_currentIndexChanged(int index);
    void on_aerenderPathEdit_textChanged(const QString &arg1);
    void on_aerenderBrowseButton_clicked();
    void on_aeCacheEdit_textChanged(const QString &arg1);
    void on_aeCacheBrowseButton_clicked();

private:
    QSettings settings;
    FFmpeg *_ffmpeg;
    bool _freezeUI;

    void refreshAeVersionBox();

};

#endif // SETTINGS_H
