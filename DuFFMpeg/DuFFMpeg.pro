#-------------------------------------------------
#
# Project created by QtCreator 2017-10-04T20:46:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DuFFMpeg
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# The current version
DEFINES += APPVERSION=\\\"1.0.0-Beta1\\\"

# For speeding up launch time when working, you can comment this line out.
# Be sure to set it back when you're finished, before committing your changes.
DEFINES += INIT_AE INIT_FFMPEG

SOURCES += \
    abstractrenderer.cpp \
    aerenderer.cpp \
    aerenderprocess.cpp \
    duexr.cpp \
    main.cpp \
    mediainfo.cpp \
    ffmpeg.cpp \
    ffqueueitem.cpp \
    ffcodec.cpp \
    ffmuxer.cpp \
    ffobject.cpp \
    ffpixformat.cpp \
    ffbaseobject.cpp \
    rendererqueue.cpp \
    uidropshadow.cpp \
    uiframeless.cpp \
    uiinputwidget.cpp \
    uimainwindow.cpp \
    uioutputwidget.cpp \
    uiqueuewidget.cpp \
    uirainbox.cpp \
    uisettingswidget.cpp \
    uisplashscreen.cpp \
    uitoolbarspacer.cpp \
    utils.cpp

HEADERS += \
    abstractrenderer.h \
    aerenderer.h \
    aerenderprocess.h \
    duexr.h \
    mediainfo.h \
    ffmpeg.h \
    ffqueueitem.h \
    ffcodec.h \
    ffmuxer.h \
    ffobject.h \
    ffpixformat.h \
    ffbaseobject.h \
    rendererqueue.h \
    uidropshadow.h \
    uiframeless.h \
    uiinputwidget.h \
    uimainwindow.h \
    uioutputwidget.h \
    uiqueuewidget.h \
    uirainbox.h \
    uisettingswidget.h \
    uisplashscreen.h \
    uitoolbarspacer.h \
    utils.h

FORMS += \
    mainwindow.ui \
    outputwidget.ui \
    settingswidget.ui \
    queuewidget.ui \
    inputwidget.ui \
    toolbarspacer.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    ico.rc

RC_FILE = ico.rc

# OpenImageIO

unix:!macx: LIBS += -lOpenImageIO \
    -lOpenImageIO_Util

INCLUDEPATH += /usr/include/OpenImageIO
DEPENDPATH += /usr/include/OpenImageIO
