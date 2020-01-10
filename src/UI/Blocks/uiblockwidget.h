#ifndef UIBLOCKWIDGET_H
#define UIBLOCKWIDGET_H

#include "ui_uiblockwidget.h"
#include "UI/uidropshadow.h"
#include "UI/Blocks/uiblockcontent.h"

class UIBlockWidget : public QWidget, private Ui::UIBlockWidget
{
    Q_OBJECT

public:
    explicit UIBlockWidget(QString title, UIBlockContent *content, QWidget *parent = nullptr);
    UIBlockContent *content();
public slots:
    void setStatus(QString status);
    void activate(bool act = true);
    void deActivate();
    void blockEnable(bool en = true);
signals:
    void activated(bool);
    void blockEnabled(bool);

private:
    UIBlockContent *_content;
};

#endif // UIBLOCKWIDGET_H
