#ifndef BLOCKLUT_H
#define BLOCKLUT_H

#include "ui_blocklut.h"
#include "UI/Blocks/blockcontentwidget.h"
#include "Renderer/cachemanager.h"

#include <QFileDialog>

class BlockLut : public BlockContentWidget,  private Ui::BlockLut
{
    Q_OBJECT

public:
    explicit BlockLut(MediaInfo *mediaInfo, QWidget *parent = nullptr);

public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void lutBox_currentIndexChanged(int index);
    void applyBox_currentIndexChanged(int index);
    void inputSpaceBox_currentIndexChanged(int index);
    void outputSpaceBox_currentIndexChanged(int index);
private:
    void updateLutInputOutputBoxes();
    FFLut *_customLut;
};

#endif // BLOCKLUT_H
