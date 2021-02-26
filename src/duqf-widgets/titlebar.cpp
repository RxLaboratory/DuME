#include "titlebar.h"

TitleBar::TitleBar(QString title, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    titleLabel->setText(title);

    reinitButton->setObjectName("windowButton");
    closeButton->setObjectName("windowButton");

    connect(reinitButton, &QToolButton::clicked, this, &TitleBar::reinitRequested);
    connect(closeButton, &QToolButton::clicked, this, &TitleBar::closeRequested);

    this->setObjectName("titleBar");
}

void TitleBar::showReinitButton(bool show)
{
    reinitButton->setVisible(show);
}
