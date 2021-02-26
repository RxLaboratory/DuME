#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "ui_settingswidget.h"

#include "duqf-widgets/appearancesettingswidget.h"
#include "duqf-widgets/titlebar.h"

class SettingsWidget : public QWidget, private Ui::SettingsWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QString title = "Settings", QWidget *parent = nullptr);
    void addPage(QWidget *ui, QString title, QIcon icon=QIcon());
    void showReinitButton(bool show);
signals:
    void reinitRequested();
    void closeRequested();

private slots:
    void on_mainList_currentRowChanged(int currentRow);

private:
    TitleBar *titleBar;
};

#endif // SETTINGSWIDGET_H
