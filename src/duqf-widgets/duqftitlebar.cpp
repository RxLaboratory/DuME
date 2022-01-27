#include "duqftitlebar.h"

DuQFTitleBar::DuQFTitleBar(QString title, bool mini, QWidget *parent) :
    QToolBar(parent)
{
    setupUi(mini);

    //this->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    titleLabel->setText(title);
    reinitButton->setToolTip("Reinitialize " + title);
    reinitButton->setStatusTip("Reinitialize " + title);
    closeButton->setToolTip("Close " + title);
    closeButton->setStatusTip("Close " + title);

    this->setWindowTitle(title + " toolbar");

    reinitButton->setObjectName("windowButton");
    closeButton->setObjectName("windowButton");

    connect(reinitButton, &QToolButton::clicked, this, &DuQFTitleBar::reinitRequested);
    connect(closeButton, &QToolButton::clicked, this, &DuQFTitleBar::closeRequested);
    connect(this, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(changeOrientation(Qt::Orientation)));

    this->setObjectName("titleBar");
}

void DuQFTitleBar::setTitle(QString title)
{
    titleLabel->setText(title);
    reinitButton->setToolTip("Reinitialize " + title);
    reinitButton->setStatusTip("Reinitialize " + title);
    closeButton->setToolTip("Close " + title);
    closeButton->setStatusTip("Close " + title);
}

void DuQFTitleBar::showReinitButton(bool show)
{
    reinitAction->setVisible(show);
}

void DuQFTitleBar::showCloseButton(bool show)
{
    closeButton->setVisible(show);
}

QAction *DuQFTitleBar::insertRight(QWidget *w)
{
    QAction *a = this->insertWidget(rightAction, w);
    m_actions.append(a);
    return a;
}

void DuQFTitleBar::insertRight(QAction *a)
{
    this->insertAction(rightAction, a);
    m_actions.append(a);
}

QAction *DuQFTitleBar::insertLeft(QWidget *w)
{
    QAction *a = this->insertWidget(leftAction, w);
    m_actions.append(a);
    return a;
}

void DuQFTitleBar::insertLeft(QAction *a)
{
    this->insertAction(leftAction, a);
    m_actions.append(a);
}

void DuQFTitleBar::changeOrientation(Qt::Orientation orientation)
{
    QSizePolicy sp;

    if (orientation == Qt::Horizontal)
    {
        sp = QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    }
    else
    {
        sp = QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    }

    for (int i = 0; i < m_actions.count(); i++)
    {
        QWidget *w = this->widgetForAction(m_actions.at(i));
        w->setSizePolicy(sp);
    }
}

void DuQFTitleBar::setupUi(bool mini)
{
    this->setObjectName(QStringLiteral("TitleBar"));

    titleLabel = new QLabel(this);

    reinitButton = new QToolButton(this);
    reinitButton->setIcon(QIcon(":/icons/reinit"));

    closeButton = new QToolButton(this);
    closeButton->setIcon(QIcon(":/icons/close"));

    this->addWidget(titleLabel);

    if (mini)
    {
        leftAction = rightAction = this->addWidget(reinitButton);
        m_actions.append(reinitAction);
    }
    else
    {
        spacer = new DuQFToolBarSpacer(this);
        leftAction = this->addWidget(spacer);
        rightAction = this->addWidget(reinitButton);
    }

    reinitAction = rightAction;
    m_actions.append(reinitAction);
    QAction *closeAction = this->addWidget(closeButton);
    m_actions.append(closeAction);
}
