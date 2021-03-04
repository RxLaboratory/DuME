#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QStackedWidget>

#include "duqf-widgets/titlebar.h"

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QString title = "Settings", QWidget *parent = nullptr);
    void addPage(QWidget *ui, QString title, QIcon icon=QIcon());
    void showReinitButton(bool show);
public slots:
    void setCurrentIndex(int index);
signals:
    void reinitRequested();
    void closeRequested();

private slots:
    void mainList_currentRowChanged(int currentRow);

private:
    void setupUi(QString title);
    QListWidget *mainList;
    QStackedWidget *mainStackWidget;
    TitleBar *titleBar;
};

#endif // SETTINGSWIDGET_H
