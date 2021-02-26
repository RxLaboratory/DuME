#include "settingswidget.h"

SettingsWidget::SettingsWidget(QString title, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    titleBar = new TitleBar(title,this);
    mainLayout->insertWidget(0, titleBar);

    connect(titleBar, &TitleBar::reinitRequested, this, &SettingsWidget::reinitRequested);
    connect(titleBar, &TitleBar::closeRequested, this, &SettingsWidget::closeRequested);
}

void SettingsWidget::addPage(QWidget *ui, QString title, QIcon icon)
{
    mainStackWidget->addWidget(ui);
    QListWidgetItem *tab = new QListWidgetItem(icon,title);
    mainList->addItem(tab);
}

void SettingsWidget::showReinitButton(bool show)
{
    titleBar->showReinitButton(show);
}

void SettingsWidget::on_mainList_currentRowChanged(int currentRow)
{
    mainStackWidget->setCurrentIndex(currentRow);
}
