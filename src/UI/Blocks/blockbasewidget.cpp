#include "blockbasewidget.h"

BlockBaseWidget::BlockBaseWidget(QString title, BlockContentWidget *content, QString ico, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    titleLabel->setText(title);
    if (ico != "") icon->setPixmap(QPixmap(ico));

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

BlockContentWidget *BlockBaseWidget::content()
{
    return _content;
}

void BlockBaseWidget::setStatus(QString status)
{
    statusLabel->setText(status);
}

void BlockBaseWidget::activate(bool act)
{
    _content->setActivated(act);
    if (!act)
        this->hide();
    else
        this->show();

    emit activated(act);
}

void BlockBaseWidget::deActivate()
{
    activate(false);
}

void BlockBaseWidget::blockEnable(bool en)
{
    if (!en)
    {
        this->hide();
    }
    emit blockEnabled(en);
}

void BlockBaseWidget::hideEvent(QHideEvent *event)
{
    emit hidden();
    event->accept();
}

void BlockBaseWidget::on_foldButton_clicked(bool checked)
{
    _content->setVisible(checked);
    if (checked)
        foldButton->setIcon(QIcon(":/icons/maximize"));
    else
        foldButton->setIcon(QIcon(":/icons/minimize"));
}
