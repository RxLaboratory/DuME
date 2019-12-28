#ifndef BLOCKRESIZE_H
#define BLOCKRESIZE_H

#include "ui_blockresize.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockResize : public UIBlockContent, private Ui::BlockResize
{
    Q_OBJECT

public:
    explicit BlockResize(MediaInfo *mediaInfo, QWidget *parent = nullptr);

public slots:
    void setActivated( bool activate );
    void update();

private slots:
    void on_videoWidthButton_valueChanged(int arg1);

    void on_videoHeightButton_valueChanged(int arg1);

private:
    void aspectRatio();
};

#endif // BLOCKRESIZE_H
