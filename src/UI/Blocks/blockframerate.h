#ifndef BLOCKFRAMERATE_H
#define BLOCKFRAMERATE_H

#include "ui_blockframerate.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockFrameRate : public UIBlockContent, private Ui::BlockFrameRate
{
    Q_OBJECT

public:
    explicit BlockFrameRate(MediaInfo *mediaInfo, QWidget *parent = nullptr);

public slots:
    void setActivated( bool activate );
    void update();
    void setFrameRate( double f );
private slots:
    void on_frameRateBox_activated(const QString &arg1);
    void on_frameRateEdit_valueChanged(double arg1);
};

#endif // BLOCKFRAMERATE_H
