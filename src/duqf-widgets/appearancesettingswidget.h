#ifndef MAINSETTINGSWIDGET_H
#define MAINSETTINGSWIDGET_H

#include "ui_appearancesettingswidget.h"

#include "duqf-app/app-style.h"

#include <QtDebug>
#include <QSettings>
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QToolButton>
#include <QSettings>
#include <QFileDialog>
#include <QStyleFactory>

class AppearanceSettingsWidget : public QWidget, private Ui::MainSettingsWidget
{
    Q_OBJECT

public:
    explicit AppearanceSettingsWidget(QWidget *parent = nullptr);
private slots:
    void on_toolsComboBox_currentIndexChanged(int index);
    void on_styleComboBox_currentIndexChanged(int index);
    void on_fontComboBox_currentFontChanged(const QFont &f);

private:
    QSettings settings;
    bool _freezeUI;
};

#endif // MAINSETTINGSWIDGET_H
