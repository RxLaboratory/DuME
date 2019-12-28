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

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    UIBlockContent *_content;
};

#endif // UIBLOCKWIDGET_H
