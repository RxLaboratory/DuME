#ifndef BLOCKVIDEOPROFILE_H
#define BLOCKVIDEOPROFILE_H

#include "ui_blockvideoprofile.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockVideoProfile : public UIBlockContent, private Ui::BlockVideoProfile
{
    Q_OBJECT

public:
    explicit BlockVideoProfile(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void setActivated( bool activate );
    void update();
private slots:
    void on_videoProfileBox_currentIndexChanged(int index);
};

#endif // BLOCKVIDEOPROFILE_H
