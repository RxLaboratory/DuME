#include "uiblockwidget.h"

UIBlockWidget::UIBlockWidget(QString title, UIBlockContent *content,QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    shadowFrame->setGraphicsEffect(new UIDropShadow);

    titleLabel->setText(title);

    _content = content;
    _content->setParent(this);
    mainLayout->addWidget(_content);
    QMenu *presets = _content->getPresets();
    if (presets->actions().count() > 0) presetsButton->setMenu(presets);
    else presetsButton->hide();

    connect(content, SIGNAL(status(QString)), this, SLOT(setStatus(QString)) );

    this->hide();
}

void UIBlockWidget::setStatus(QString status)
{
    statusLabel->setText(status);
}

void UIBlockWidget::showEvent(QShowEvent *event)
{
    _content->setActivated(true);
    emit activated(true);
    event->accept();
}

void UIBlockWidget::hideEvent(QHideEvent *event)
{
    _content->setActivated(false);
    emit activated(false);
    event->accept();
}

void UIBlockWidget::on_closeButton_clicked()
{
    this->hide();
}
