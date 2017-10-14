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
    void changeFFmpegPath(QString path);

signals:
    void ffmpegPathChanged(QString);

private slots:
    void on_ffmpegPathEdit_textChanged(const QString &arg1);
    void on_ffmpegBrowseButton_clicked();

private:
    QSettings *settings;

};

#endif // SETTINGS_H
