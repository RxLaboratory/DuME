#ifndef BLOCKMOTIONINTERPOLATION_H
#define BLOCKMOTIONINTERPOLATION_H

#include "ui_blockmotioninterpolation.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockMotionInterpolation : public BlockContentWidget, private Ui::BlockMotionInterpolation
{
    Q_OBJECT

public:
    explicit BlockMotionInterpolation(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_interpolationBox_currentIndexChanged(int index);
    void on_estimationBox_currentIndexChanged(int index);
    void on_algorithmBox_currentIndexChanged(int index);
    void on_sceneDetectionBox_clicked(bool checked);
private:
    void updateInterpolationUI();
};

#endif // BLOCKMOTIONINTERPOLATION_H
