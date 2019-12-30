#ifndef BLOCKSAMPLING_H
#define BLOCKSAMPLING_H

#include "ui_blocksampling.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockSampling : public UIBlockContent, private Ui::BlockSampling
{
    Q_OBJECT

public:
    explicit BlockSampling(MediaInfo *mediaInfo,QWidget *parent = nullptr);
    void setSampling(int s);
public slots:
    void setActivated( bool activate );
    void update();
private slots:
    void on_samplingBox_currentIndexChanged(int index);
};

#endif // BLOCKSAMPLING_H
