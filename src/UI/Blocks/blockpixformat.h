#ifndef BLOCKPIXFORMAT_H
#define BLOCKPIXFORMAT_H

#include "ui_blockpixformat.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockPixFormat : public UIBlockContent, private Ui::BlockPixFormat
{
    Q_OBJECT

public:
    explicit BlockPixFormat(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void setActivated( bool activate );
    void update();
private slots:
    void on_pixFmtFilterBox_currentIndexChanged(int index);
    void on_pixFmtBox_currentIndexChanged(int index);
private:
    void listPixFormats(bool init = false);
    void setDefaultPixFormat();
    void setPixFormat(QString name);
};

#endif // BLOCKPIXFORMAT_H
