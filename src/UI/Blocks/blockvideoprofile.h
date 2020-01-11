#ifndef BLOCKVIDEOPROFILE_H
#define BLOCKVIDEOPROFILE_H

#include "ui_blockvideoprofile.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockVideoProfile : public BlockContentWidget, private Ui::BlockVideoProfile
{
    Q_OBJECT

public:
    explicit BlockVideoProfile(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_videoProfileBox_currentIndexChanged(int index);
    void on_videoLevelBox_currentIndexChanged(int index);
};

#endif // BLOCKVIDEOPROFILE_H
