#ifndef CACHESETTINGSWIDGET_H
#define CACHESETTINGSWIDGET_H

#include "ui_cachesettingswidget.h"

#include "Renderer/cachemanager.h"

#include <QSettings>
#include <QFileDialog>
#include <QDir>
#include <QTemporaryDir>
#include <QFile>

class CacheSettingsWidget : public QWidget, private Ui::CacheSettingsWidget
{
    Q_OBJECT

public:
    explicit CacheSettingsWidget(QWidget *parent = nullptr);

private slots:
    void on_cacheEdit_textChanged(const QString &arg1);
    void on_cacheBrowseButton_clicked();

    void on_openButton_clicked();

private:
    QSettings settings;
    bool _freezeUI;
};

#endif // CACHESETTINGSWIDGET_H
