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
    connect(content, SIGNAL(blockEnabled(bool)), this, SLOT(blockEnable(bool)) );
    connect(closeButton, SIGNAL(clicked()), this, SLOT (deActivate()) );

    this->hide();
}

UIBlockContent *UIBlockWidget::content()
{
    return _content;
}

void UIBlockWidget::setStatus(QString status)
{
    statusLabel->setText(status);
}

void UIBlockWidget::activate(bool act)
{
    _content->activate(act);
    if (!act) this->hide();
    else this->show();
    emit activated(act);
}

void UIBlockWidget::deActivate()
{
    activate(false);
}

void UIBlockWidget::blockEnable(bool en)
{
    if (!en)
    {
        this->hide();
    }
    emit blockEnabled(en);
}
