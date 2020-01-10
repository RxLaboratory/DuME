#ifndef BLOCKMAPPING_H
#define BLOCKMAPPING_H

#include <QWidget>

#include "ui_blockmapping.h"
#include "UI/Blocks/uiblockcontent.h"
#include "UI/streamreferencewidget.h"

class BlockMapping : public UIBlockContent, private Ui::BlockMapping
{
    Q_OBJECT

public:
    explicit BlockMapping(MediaInfo *mediaInfo, MediaList *inputMedias, QWidget *parent = nullptr);

public slots:
    void activate( bool blockEnabled );
    void update();

private slots:
    void changeStream(int index, int streamId);
    void changeMedia(int index, int mediaId);
    void removeStreamWidget(int id);
    void on_actionAdd_triggered();

private:
    QList<StreamReferenceWidget *> _streamWidgets;
    void addStreamWidget(int mediaId = -1, int streamId = -1);

};

#endif // BLOCKMAPPING_H
