#ifndef AESETTINGSWIDGET_H
#define AESETTINGSWIDGET_H

#include "ui_aesettingswidget.h"

#include "AfterEffects/aftereffects.h"
#include "Renderer/cachemanager.h"

#include <QSettings>
#include <QFileDialog>
#include <QDir>
#include <QTemporaryDir>
#include <QFile>

class AESettingsWidget : public QWidget, private Ui::AESettingsWidget
{
    Q_OBJECT

public:
    explicit AESettingsWidget(AfterEffects *ae, QWidget *parent = nullptr);

private slots:
    void on_aeVersionBox_currentIndexChanged(int index);
    void on_aerenderPathEdit_textChanged(const QString &arg1);
    void on_aerenderBrowseButton_clicked();
    void on_aeCacheEdit_textChanged(const QString &arg1);
    void on_aeCacheBrowseButton_clicked();
    void updateAe();

private:
    QSettings settings;
    AfterEffects *_ae;
    bool _freezeUI;
    void refreshAeVersionBox();
};

#endif // AESETTINGSWIDGET_H
