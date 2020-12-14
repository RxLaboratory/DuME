#ifndef DUMESETTINGS_H
#define DUMESETTINGS_H

#include "ui_dumesettingswidget.h"

#include "global.h"

#include "AfterEffects/aftereffects.h"

#include <QSettings>
#include <QFileDialog>
#include <QDir>
#include <QTemporaryDir>
#include <QFile>


class DuMESettingsWidget : public QWidget, private Ui::DuMESettingsWidget
{
    Q_OBJECT

public:
    explicit DuMESettingsWidget(AfterEffects *ae, QWidget *parent = nullptr);

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
    void on_resetDefaultsButton_clicked();

private:
    QSettings settings;
    AfterEffects *_ae;
    bool _freezeUI;
    void refreshAeVersionBox();

};

#endif // DUMESETTINGS_H
