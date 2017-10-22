#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_settingswidget.h"

#include <QSettings>
#include <QFileDialog>

class SettingsWidget : public QWidget, private Ui::SettingsWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QSettings *s, QWidget *parent = 0);

signals:
    void ffmpegPathChanged(QString);
    void presetsPathChanged(QString);

private slots:
    void on_ffmpegBrowseButton_clicked();
    void on_ffmpegPathEdit_editingFinished();

    void on_userPresetsBrowseButton_clicked();

    void on_userPresetsPathEdit_editingFinished();
private:
    QSettings *settings;

};

#endif // SETTINGS_H
