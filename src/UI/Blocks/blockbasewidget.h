#ifndef UIBLOCKWIDGET_H
#define UIBLOCKWIDGET_H

#include "ui_uiblockwidget.h"
#include "UI/dropshadow.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockBaseWidget : public QWidget, private Ui::UIBlockWidget
{
    Q_OBJECT

public:
    explicit BlockBaseWidget(QString title, BlockContentWidget *content, QWidget *parent = nullptr);
    BlockContentWidget *content();
public slots:
    void setStatus(QString status);
    void activate(bool act = true);
    void deActivate();
    void blockEnable(bool en = true);
signals:
    void activated(bool);
    void blockEnabled(bool);
    void hidden();

protected:
    void hideEvent(QHideEvent *event);

private:
    BlockContentWidget *_content;
};

#endif // UIBLOCKWIDGET_H
