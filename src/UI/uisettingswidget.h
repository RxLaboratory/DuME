#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_settingswidget.h"

#include "FFmpeg/ffmpeg.h"
#include "AfterEffects/aftereffects.h"

#include <QSettings>
#include <QFileDialog>
#include <QDir>
#include <QTemporaryDir>
#include <QFile>


class UISettingsWidget : public QWidget, private Ui::SettingsWidget
{
    Q_OBJECT

public:
    explicit UISettingsWidget(FFmpeg *ffmpeg, AfterEffects *ae, QWidget *parent = nullptr);

signals:
    void presetsPathChanged();

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
    AfterEffects *_ae;
    bool _freezeUI;

    void refreshAeVersionBox();

};

#endif // SETTINGS_H
