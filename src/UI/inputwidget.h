#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "ui_inputwidget.h"

#include "Renderer/mediainfo.h"

//BLOCKS
#include "UI/Blocks/blockbasewidget.h"
#include "UI/Blocks/blockframerate.h"
#include "UI/Blocks/blockexr.h"
#include "UI/Blocks/blockcolor.h"
#include "UI/Blocks/blockaecomp.h"
#include "UI/Blocks/blockaethreads.h"
#include "UI/Blocks/blockcustom.h"

#include <QFileDialog>
#include <QSettings>
#include <QThread>
#include <QTime>

class InputWidget : public QWidget, private Ui::InputWidget
{
    Q_OBJECT

public:
    explicit InputWidget(FFmpeg *ff, int id = -1, QWidget *parent = nullptr);
    /**
     * @brief mediaInfo Gives a pointer to the current input MediaInfo
     * @return The current MediaInfo
     */
    MediaInfo *mediaInfo();
    /**
     * @brief getMediaInfo Gives a pointer to a copy of the current input MediaInfo
     * @return The new copy
     */
    MediaInfo *getMediaInfo();
    void openFile(QString file);
    void openFile(QUrl file);

signals:
    void newMediaLoaded(MediaInfo *);

private slots:
    void on_inputBrowseButton_clicked();
    void on_inputEdit_editingFinished();
    void on_actionAddCustom_triggered();
    void customParamActivated(bool activated);
private:
    // BLOCKS
    BlockBaseWidget *addBlock(BlockContentWidget *content, QAction *action);
    BlockBaseWidget *blockFrameRate;
    BlockFrameRate *blockFrameRateContent;
    BlockBaseWidget *blockEXR;
    BlockEXR *blockEXRContent;
    BlockBaseWidget *blockColor;
    BlockColor *blockColorContent;
    BlockBaseWidget *blockAEComp;
    BlockAEComp *blockAECompContent;
    BlockBaseWidget *blockAEThreads;
    BlockAEThreads *blockAEThreadsContent;

    // MENUS
    QMenu *blocksMenu;

    void addNewParam(QString name = "",QString value = "");

    MediaInfo *_mediaInfo;
    QList<BlockBaseWidget *> _customParams;
    void updateOptions();
    void updateCustomParams();
};

#endif // INPUTWIDGET_H
