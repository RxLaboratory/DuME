#ifndef TOOLBARSPACER_H
#define TOOLBARSPACER_H

#include "ui_toolbarspacer.h"

class UIToolBarSpacer : public QWidget, private Ui::ToolBarSpacer
{
    Q_OBJECT

public:
    explicit UIToolBarSpacer(QWidget *parent = 0);
};

#endif // TOOLBARSPACER_H
