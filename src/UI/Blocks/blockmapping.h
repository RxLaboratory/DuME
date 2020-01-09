#ifndef BLOCKMAPPING_H
#define BLOCKMAPPING_H

#include <QWidget>

#include "ui_blockmapping.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockMapping : public UIBlockContent, private Ui::BlockMapping
{
    Q_OBJECT

public:
    explicit BlockMapping(MediaInfo *mediaInfo, QWidget *parent = nullptr);

public slots:
    void setActivated( bool activate );
    void update();

};

#endif // BLOCKMAPPING_H
