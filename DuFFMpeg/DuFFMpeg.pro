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

DEFINES += APPVERSION=\\\"0.0.5-Alpha\\\"

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    toolbarspacer.cpp \
    settingswidget.cpp \
    ffmpeg.cpp \
    ffqueueitem.cpp \
    ffcodec.cpp \
    ffmediainfo.cpp \
    queuewidget.cpp \
    inputwidget.cpp \
    outputwidget.cpp \
    rainboxui.cpp \
    frameless.cpp \
    ffmuxer.cpp \
    ffobject.cpp

HEADERS += \
    mainwindow.h \
    toolbarspacer.h \
    settingswidget.h \
    ffmpeg.h \
    ffqueueitem.h \
    ffcodec.h \
    ffmediainfo.h \
    queuewidget.h \
    inputwidget.h \
    outputwidget.h \
    rainboxui.h \
    frameless.h \
    ffmuxer.h \
    ffobject.h

FORMS += \
    mainwindow.ui \
    toolbarspacer.ui \
    settingswidget.ui \
    queuewidget.ui \
    inputwidget.ui \
    outputwidget.ui

RESOURCES += \
    resources.qrc
