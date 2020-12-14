#include "settingswidget.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

void SettingsWidget::addPage(QWidget *ui, QString title, QIcon icon)
{
    mainStackWidget->addWidget(ui);
    QListWidgetItem *tab = new QListWidgetItem(icon,title);
    mainList->addItem(tab);
}

void SettingsWidget::on_mainList_currentRowChanged(int currentRow)
{
    mainStackWidget->setCurrentIndex(currentRow);
}
