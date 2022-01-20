#include "duqfdocktitle.h"

DuQFDockTitle::DuQFDockTitle(QString title, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,3,0,3);
    vlayout->setSpacing(0);

    // include in a frame for the BG
    QFrame *mainFrame = new QFrame(this);

    QHBoxLayout *layout = new QHBoxLayout(mainFrame);
    layout->setContentsMargins(3,0,3,0);
    layout->setSpacing(3);

    layout->addStretch();

    _iconLabel = new QLabel(this);
    layout->addWidget(_iconLabel);
    _iconLabel->setScaledContents(true);
    _iconLabel->setMaximumSize(16, 16);

    _titleLabel = new QLabel(title, this);
    layout->addWidget(_titleLabel);

    layout->addStretch();

    QToolButton *closeButton = new QToolButton(this);
    closeButton->setIcon(QIcon(":/icons/close"));
    closeButton->setObjectName("windowButton");

    layout->addWidget(closeButton);

    mainFrame->setLayout(layout);
    vlayout->addWidget(mainFrame);

    this->setLayout(vlayout);

    connect(closeButton, &QToolButton::clicked, this, &DuQFDockTitle::closeDockWidget);
}

void DuQFDockTitle::setTitle(QString title)
{
    _titleLabel->setText(title);
}

void DuQFDockTitle::setIcon(QString icon)
{
    _iconLabel->setPixmap(QPixmap(icon));
}

void DuQFDockTitle::closeDockWidget()
{
    parentWidget()->hide();
}
