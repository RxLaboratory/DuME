#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "ui_inputwidget.h"

#include "Renderer/mediainfo.h"

//BLOCKS
#include "UI/Blocks/blockbasewidget.h"
#include "UI/Blocks/blockframerate.h"
#include "UI/Blocks/blockexr.h"
#include "UI/Blocks/blockcolorinput.h"
#include "UI/Blocks/blockaecomp.h"
#include "UI/Blocks/blockaethreads.h"
#include "UI/Blocks/blockcustom.h"
#include "UI/Blocks/blocktimerange.h"

#include <QFileDialog>
#include <QSettings>
#include <QThread>
#include <QTime>

class InputWidget : public QWidget, private Ui::InputWidget
{
    Q_OBJECT

public:
    explicit InputWidget(int id = -1, QWidget *parent = nullptr);
    /**
     * @brief mediaInfo Gives a pointer to the current input MediaInfo
     * @return The current MediaInfo
     */
    MediaInfo *mediaInfo();
    void openFile(QString file);
    void openFile(QUrl file);
    bool hasMedia();

private slots:
    void on_inputBrowseButton_clicked();
    void on_inputEdit_editingFinished();
    void updateOptions();
    void updateInfo();
private:
    // BLOCKS
    BlockBaseWidget *addBlock(BlockContentWidget *content, QAction *action, QString icon = "");
    BlockBaseWidget *blockFrameRate;
    BlockFrameRate *blockFrameRateContent;
    BlockBaseWidget *blockEXR;
    BlockEXR *blockEXRContent;
    BlockBaseWidget *blockColor;
    BlockColorInput *blockColorContent;
    BlockBaseWidget *blockAEComp;
    BlockAEComp *blockAECompContent;
    BlockBaseWidget *blockAEThreads;
    BlockAEThreads *blockAEThreadsContent;
    BlockBaseWidget *blockTimeRange;
    BlockTimeRange *blockTimeRangeContent;
    BlockBaseWidget *blockCustom;
    BlockCustom *blockCustomContent;

    // MENUS
    QMenu *blocksMenu;

    MediaInfo *_mediaInfo;
    QList<BlockBaseWidget *> _customParams;
};

#endif // INPUTWIDGET_H
