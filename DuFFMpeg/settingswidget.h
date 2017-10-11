#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_settingswidget.h"

#include <QSettings>

class SettingsWidget : public QWidget, private Ui::SettingsWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QSettings *s, QWidget *parent = 0);

private slots:
    void on_ffmpegPathEdit_textChanged(const QString &arg1);

private:
    QSettings *settings;

};

#endif // SETTINGS_H
