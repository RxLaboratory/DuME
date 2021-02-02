#ifndef BLOCKCUSTOM_H
#define BLOCKCUSTOM_H

#include "ui_blockcustom.h"
#include "UI/Blocks/blockcontentwidget.h"
#include "UI/ffmpegparamwidget.h"

class BlockCustom : public BlockContentWidget, private Ui::BlockCustom
{
    Q_OBJECT

public:
    explicit BlockCustom(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void updateParams();
    void addParam();
    void addParam(QString p, QString v);
    void removeParams();
    void on_actionAddParameter_triggered();

private:
    QList<FFmpegParamWidget*> _paramWidgets;
};

#endif // BLOCKCUSTOM_H
