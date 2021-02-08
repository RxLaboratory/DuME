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
    explicit AESettingsWidget(QWidget *parent = nullptr);

private slots:
    void aeVersionBox_currentIndexChanged(int index);
    void aerenderPathEdit_textChanged(const QString &arg1);
    void aerenderBrowseButton_clicked();
    void updateAe();

private:
    QSettings settings;
    AfterEffects *_ae;
    void refreshAeVersionBox();
};

#endif // AESETTINGSWIDGET_H
