#ifndef BLOCKCUSTOM_H
#define BLOCKCUSTOM_H

#include "ui_blockcustom.h"

class BlockCustom : public QWidget, private Ui::BlockCustom
{
    Q_OBJECT

public:
    explicit BlockCustom(QWidget *parent = nullptr);
    explicit BlockCustom(QString p, QString v, QWidget *parent = nullptr);
    void setParam(QString p);
    void setValue(QString v);
    QString param();
    QString value();
};

#endif // BLOCKCUSTOM_H
