#ifndef BLOCKAETHREADS_H
#define BLOCKAETHREADS_H

#include "ui_blockaethreads.h"
#include "UI/Blocks/blockcontentwidget.h"
#include <QThread>

class BlockAEThreads : public BlockContentWidget, private Ui::BlockAEThreads
{
    Q_OBJECT

public:
    explicit BlockAEThreads(MediaInfo *mediaInfo, QWidget *parent = nullptr);
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_threadsBox_valueChanged(int arg1);
};

#endif // BLOCKAETHREADS_H
