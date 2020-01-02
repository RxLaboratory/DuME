#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "ui_inputwidget.h"

#include "Renderer/mediainfo.h"

//BLOCKS
#include "UI/Blocks/uiblockwidget.h"
#include "UI/Blocks/blockframerate.h"
#include "UI/Blocks/blockexr.h"
#include "UI/Blocks/blockcolor.h"
#include "UI/Blocks/blockaecomp.h"

#include <QFileDialog>
#include <QSettings>
#include <QThread>
#include <QTime>

class UIInputWidget : public QWidget, private Ui::InputWidget
{
    Q_OBJECT

public:
    explicit UIInputWidget(FFmpeg *ff, QWidget *parent = nullptr);
    MediaInfo *getMediaInfo();
    void openFile(QString file);
    void openFile(QUrl file);

signals:
    void newMediaLoaded(MediaInfo *);

private slots:
    void on_inputBrowseButton_clicked();
    void on_addParamButton_clicked();
    void on_inputEdit_editingFinished();
    void on_threadsButton_toggled(bool checked);
    void on_threadsBox_valueChanged(int arg1);

private:
    // BLOCKS
    UIBlockWidget *addBlock(UIBlockContent *content, QAction *action);
    UIBlockWidget *blockFrameRate;
    BlockFrameRate *blockFrameRateContent;
    UIBlockWidget *blockEXR;
    BlockEXR *blockEXRContent;
    UIBlockWidget *blockColor;
    BlockColor *blockColorContent;
    UIBlockWidget *blockAEComp;
    BlockAEComp *blockAECompContent;


    // MENUS
    QMenu *blocksMenu;

    MediaInfo *_mediaInfo;
    QList<QLineEdit *> _customParamEdits;
    QList<QLineEdit *> _customValueEdits;
    void updateOptions();

};

#endif // INPUTWIDGET_H
