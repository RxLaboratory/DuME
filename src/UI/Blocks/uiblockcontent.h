#ifndef UIBLOCKCONTENT_H
#define UIBLOCKCONTENT_H

#include <QWidget>
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

class UIBlockContent : public QWidget
{
    Q_OBJECT
public:
    explicit UIBlockContent(MediaInfo *mediaInfo, QWidget *parent = nullptr);

public slots:
    // reimplement this in the blocks to update mediainfo when (de)activated
    virtual void setActivated( bool activate );

signals:

private:
    bool _activated;
    MediaInfo *_mediaInfo;

};

#endif // UIBLOCKCONTENT_H
