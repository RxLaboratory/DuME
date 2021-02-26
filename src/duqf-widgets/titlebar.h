#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "ui_titlebar.h"

class TitleBar : public QWidget, private Ui::TitleBar
{
    Q_OBJECT

public:
    explicit TitleBar(QString title, QWidget *parent = nullptr);
    void showReinitButton(bool show);

signals:
    void reinitRequested();
    void closeRequested();
};

#endif // TITLEBAR_H
