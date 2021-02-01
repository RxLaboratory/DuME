#ifndef BLOCKTIMERANGE_H
#define BLOCKTIMERANGE_H

#include "ui_blocktimerange.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockTimeRange : public BlockContentWidget, private Ui::BlockTimeRange
{
    Q_OBJECT

public:
    explicit BlockTimeRange(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_inEdit_editingFinished();
    void on_outEdit_editingFinished();
};

#endif // BLOCKTIMERANGE_H
