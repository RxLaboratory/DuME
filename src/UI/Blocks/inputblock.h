#ifndef INPUTBLOCK_H
#define INPUTBLOCK_H

#include <QWidget>
#include <QTextBrowser>
#include <QMimeData>

#include "duqf-widgets/duqffolderselectorwidget.h"

#include "Renderer/mediainfo.h"

#include "UI/Blocks/blockbasewidget.h"
#include "UI/Blocks/blockframerate.h"
#include "UI/Blocks/blockexr.h"
#include "UI/Blocks/blockcolorinput.h"
#include "UI/Blocks/blockaecomp.h"
#include "UI/Blocks/blockaethreads.h"
#include "UI/Blocks/blockcustom.h"
#include "UI/Blocks/blocktimerange.h"

class InputBlock : public QWidget
{
    Q_OBJECT

public:
    explicit InputBlock(MediaInfo *mediaInfo, QWidget *parent = nullptr);
    MediaInfo *mediaInfo() const;

public slots:
    void openFile(QString file);
    void openFile(QUrl file);

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);

private slots:
    void updateInfo();
    void updateOptions();

private:
    void setupUi();
    void connectEvents();
    BlockBaseWidget *addBlock(BlockContentWidget *content, QAction *action, QString icon = "");

    DuQFFolderSelectorWidget *ui_fileSelector;
    QTextBrowser *ui_mediaInfosText;
    QToolButton *ui_addBlockButton;
    QWidget *ui_optionsContentWidget;
    QVBoxLayout *ui_optionsContentLayout;
    QMenu *ui_blockMenu;
    QAction *actionFramerate;
    QAction *actionEXR;
    QAction *actionColor;
    QAction *actionAfter_Effects_composition;
    QAction *actionAddCustom;
    QAction *actionTimeRange;

    BlockBaseWidget *blockFrameRate;
    BlockBaseWidget *blockEXR;
    BlockBaseWidget *blockColor;
    BlockBaseWidget *blockAEComp;
    BlockBaseWidget *blockTimeRange;
    BlockBaseWidget *blockCustom;

    MediaInfo *m_mediaInfo;
};

#endif // INPUTBLOCK_H
