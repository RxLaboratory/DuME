#ifndef BLOCKRESIZE_H
#define BLOCKRESIZE_H

#include "ui_blockresize.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockResize : public BlockContentWidget, private Ui::BlockResize
{
    Q_OBJECT

public:
    explicit BlockResize(MediaInfo *mediaInfo, QWidget *parent = nullptr);

public slots:
    void activate( bool blockEnabled );
    void update();
    void setWidth( int w );
    void setHeight( int h );
private slots:
    void on_videoWidthButton_editingFinished( );
    void on_videoHeightButton_editingFinished( );
    void on_modeBox_currentIndexChanged(const QString &arg1);
    void on_actionHD720_triggered();
    void on_actionHD1080_triggered();
    void on_action2KFlat_triggered();
    void on_action2KScope_triggered();
    void on_actionDSM2K_triggered();
    void on_action4KFlat_triggered();
    void on_action4KScope_triggered();
    void on_actionDSM4K_triggered();
    void on_action4KUHD_triggered();
    void on_action8KUHD_triggered();

    void on_algorithmBox_currentIndexChanged(int index);

private:
    QString aspectRatio();
    void checkSizes();
    void setSize(int w, int h);
    double _currentRatio;
};

#endif // BLOCKRESIZE_H
