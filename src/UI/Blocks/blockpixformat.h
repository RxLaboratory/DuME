#ifndef BLOCKPIXFORMAT_H
#define BLOCKPIXFORMAT_H

#include "ui_blockpixformat.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockPixFormat : public BlockContentWidget, private Ui::BlockPixFormat
{
    Q_OBJECT

public:
    explicit BlockPixFormat(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_pixFmtFilterBox_currentIndexChanged(int index);
    void on_pixFmtBox_currentIndexChanged(int index);
private:
    void listPixFormats();
    void filterPixFormats(bool resetPrevious = true);
    void setDefaultPixFormat();
    void setPixFormat(QString name,  bool tryWithoutFilter = true );
    QList<FFPixFormat *> _pixFormats;
    void setFilter(QString name);
};

#endif // BLOCKPIXFORMAT_H
