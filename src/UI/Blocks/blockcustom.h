#ifndef BLOCKCUSTOM_H
#define BLOCKCUSTOM_H

#include "ui_blockcustom.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockCustom : public UIBlockContent, private Ui::BlockCustom
{
    Q_OBJECT

public:
    explicit BlockCustom(MediaInfo *mediaInfo, QWidget *parent = nullptr);
    explicit BlockCustom(MediaInfo *mediaInfo, QString p, QString v, QWidget *parent = nullptr);
    void setParam(QString p);
    void setValue(QString v);
    QString param();
    QString value();
};

#endif // BLOCKCUSTOM_H
