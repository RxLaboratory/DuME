#ifndef FFMPEGSETTINGSWIDGET_H
#define FFMPEGSETTINGSWIDGET_H

#include "ui_ffmpegsettingswidget.h"

#include "global.h"

#include <QSettings>
#include <QFileDialog>

class FFmpegSettingsWidget : public QWidget, private Ui::FFmpegSettingsWidget
{
    Q_OBJECT

public:
    explicit FFmpegSettingsWidget(QWidget *parent = nullptr);

signals:
    void presetsPathChanged();

private slots:
    void on_ffmpegBrowseButton_clicked();
    void on_ffmpegPathEdit_editingFinished();
    void on_userPresetsBrowseButton_clicked();
    void on_userPresetsPathEdit_editingFinished();
    void ffmpegStatus(MediaUtils::RenderStatus status);
private:
    QSettings settings;
    bool _freezeUI;
};

#endif // FFMPEGSETTINGSWIDGET_H
