#ifndef UIBLOCKCONTENT_H
#define UIBLOCKCONTENT_H

#include <QWidget>
#include <QMenu>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QResizeEvent>

#include "Renderer/mediainfo.h"
#include "Renderer/medialist.h"

class UIBlockContent : public QWidget
{
    Q_OBJECT
public:
    explicit UIBlockContent(MediaInfo *mediaInfo, MediaList *inputMedias, QWidget *parent = nullptr);
    UIBlockContent(MediaInfo *mediaInfo, QWidget *parent = nullptr);
    QMenu* getPresets() const;

public slots:
    // reimplement this in the blocks to update the MediaInfo when (de)activated
    virtual void activate( bool act );
    // reimplement this in the blocks to update the UI from the MediaInfo when it changes
    virtual void update();

signals:
    void status(QString);
    //emitted to make this block (un)available
    void blockEnabled(bool);

protected:
    MediaInfo *_mediaInfo;
    MediaList *_inputMedias;
    bool _freezeUI;
    QMenu *_presets;

private slots:
    void changed();

private:
    bool _activated;

};

#endif // UIBLOCKCONTENT_H
