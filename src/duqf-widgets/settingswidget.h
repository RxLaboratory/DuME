#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "ui_settingswidget.h"

#include "duqf-widgets/appearancesettingswidget.h"

class SettingsWidget : public QWidget, private Ui::SettingsWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    void addPage(QWidget *ui, QString title, QIcon icon=QIcon());

private slots:
    void on_mainList_currentRowChanged(int currentRow);
};

#endif // SETTINGSWIDGET_H
