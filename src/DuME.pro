#-------------------------------------------------
#
# Project created by QtCreator 2017-10-04T20:46:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DuME
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

# For speeding up launch time when working, you can comment this line out.
# Be sure to set it back when you're finished, before committing your changes.
DEFINES += INIT_AE INIT_FFMPEG

SOURCES += \
    AfterEffects/aftereffects.cpp \
    AfterEffects/aftereffectsversion.cpp \
    DuF/duapplication.cpp \
    DuF/dusplashscreen.cpp \
    DuF/duui.cpp \
    DuF/duversion.cpp \
    FFmpeg/fflanguage.cpp \
    FFmpeg/ffprofile.cpp \
    Renderer/abstractrenderer.cpp \
    AfterEffects/aerenderer.cpp \
    Renderer/abstractrendererinfo.cpp \
    Renderer/audioinfo.cpp \
    Renderer/medialist.cpp \
    Renderer/preset.cpp \
    Renderer/presetmanager.cpp \
    Renderer/queueitem.cpp \
    Renderer/renderqueue.cpp \
    FFmpeg/ffmpegrenderer.cpp \
    Renderer/streamreference.cpp \
    Renderer/videoinfo.cpp \
    UI/Blocks/blockaecomp.cpp \
    UI/Blocks/blockaethreads.cpp \
    UI/Blocks/blockalpha.cpp \
    UI/Blocks/blockaudiobitrate.cpp \
    UI/Blocks/blockaudiocodec.cpp \
    UI/Blocks/blockbasewidget.cpp \
    UI/Blocks/blockcolor.cpp \
    UI/Blocks/blockcontentwidget.cpp \
    UI/Blocks/blockcustom.cpp \
    UI/Blocks/blockexr.cpp \
    UI/Blocks/blockframerate.cpp \
    UI/Blocks/blockloops.cpp \
    UI/Blocks/blockmapping.cpp \
    UI/Blocks/blockpixformat.cpp \
    UI/Blocks/blockresize.cpp \
    UI/Blocks/blocksampling.cpp \
    UI/Blocks/blockstartnumber.cpp \
    UI/Blocks/blockvideobitrate.cpp \
    UI/Blocks/blockvideocodec.cpp \
    UI/Blocks/blockvideoprofile.cpp \
    UI/bigcombobox.cpp \
    UI/datacombobox.cpp \
    UI/dropshadow.cpp \
    UI/framelesswindow.cpp \
    UI/inputwidget.cpp \
    UI/mainwindow.cpp \
    UI/outputwidget.cpp \
    UI/queuewidget.cpp \
    UI/settingswidget.cpp \
    UI/streamreferencewidget.cpp \
    UI/toolbarspacer.cpp \
    FFmpeg/ffcolorprofile.cpp \
    global.cpp \
    languagelist.cpp \
    main.cpp \
    Renderer/mediainfo.cpp \
    FFmpeg/ffmpeg.cpp \
    FFmpeg/ffcodec.cpp \
    FFmpeg/ffmuxer.cpp \
    FFmpeg/ffpixformat.cpp \
    FFmpeg/ffbaseobject.cpp \
    utils.cpp

HEADERS += \
    AfterEffects/aftereffects.h \
    AfterEffects/aftereffectsversion.h \
    DuF/duapplication.h \
    DuF/dusplashscreen.h \
    DuF/duui.h \
    DuF/duversion.h \
    DuF/version.h \
    FFmpeg/fflanguage.h \
    FFmpeg/ffprofile.h \
    Renderer/abstractrenderer.h \
    AfterEffects/aerenderer.h \
    Renderer/abstractrendererinfo.h \
    Renderer/audioinfo.h \
    Renderer/medialist.h \
    Renderer/preset.h \
    Renderer/presetmanager.h \
    Renderer/queueitem.h \
    Renderer/renderqueue.h \
    FFmpeg/ffmpegrenderer.h \
    Renderer/mediainfo.h \
    FFmpeg/ffmpeg.h \
    FFmpeg/ffcodec.h \
    FFmpeg/ffmuxer.h \
    FFmpeg/ffpixformat.h \
    FFmpeg/ffbaseobject.h \
    Renderer/streamreference.h \
    Renderer/videoinfo.h \
    UI/Blocks/blockaecomp.h \
    UI/Blocks/blockaethreads.h \
    UI/Blocks/blockalpha.h \
    UI/Blocks/blockaudiobitrate.h \
    UI/Blocks/blockaudiocodec.h \
    UI/Blocks/blockbasewidget.h \
    UI/Blocks/blockcolor.h \
    UI/Blocks/blockcontentwidget.h \
    UI/Blocks/blockexr.h \
    UI/Blocks/blockframerate.h \
    UI/Blocks/blockloops.h \
    UI/Blocks/blockmapping.h \
    UI/Blocks/blockpixformat.h \
    UI/Blocks/blockresize.h \
    UI/Blocks/blocksampling.h \
    UI/Blocks/blockstartnumber.h \
    UI/Blocks/blockvideobitrate.h \
    UI/Blocks/blockvideocodec.h \
    UI/Blocks/blockvideoprofile.h \
    UI/Blocks/blockcustom.h \
    UI/bigcombobox.h \
    UI/datacombobox.h \
    UI/dropshadow.h \
    UI/framelesswindow.h \
    UI/inputwidget.h \
    UI/mainwindow.h \
    UI/outputwidget.h \
    UI/queuewidget.h \
    UI/settingswidget.h \
    UI/streamreferencewidget.h \
    UI/toolbarspacer.h \
    FFmpeg/ffcolorprofile.h \
    global.h \
    languagelist.h \
    utils.h

FORMS += \
    UI/Blocks/blockaecomp.ui \
    UI/Blocks/blockaethreads.ui \
    UI/Blocks/blockalpha.ui \
    UI/Blocks/blockaudiobitrate.ui \
    UI/Blocks/blockaudiocodec.ui \
    UI/Blocks/blockcolor.ui \
    UI/Blocks/blockexr.ui \
    UI/Blocks/blockframerate.ui \
    UI/Blocks/blockloops.ui \
    UI/Blocks/blockmapping.ui \
    UI/Blocks/blockpixformat.ui \
    UI/Blocks/blockresize.ui \
    UI/Blocks/blocksampling.ui \
    UI/Blocks/blockstartnumber.ui \
    UI/Blocks/blockvideobitrate.ui \
    UI/Blocks/blockvideocodec.ui \
    UI/Blocks/blockvideoprofile.ui \
    UI/Blocks/blockcustom.ui \
    UI/Blocks/uiblockwidget.ui \
    UI/mainwindow.ui \
    UI/outputwidget.ui \
    UI/settingswidget.ui \
    UI/queuewidget.ui \
    UI/inputwidget.ui \
    UI/streamreferencewidget.ui \
    UI/toolbarspacer.ui

RESOURCES += resources.qrc

ICON = resources/icons/appIcon.icns


# OS Specific configurations
win* {
    # Add version and other metadata
    DISTFILES += app.rc
    RC_FILE = app.rc
    !build_pass:touch($$RC_FILE, DuF/version.h)
    # Enable console output
    CONFIG += console
} else:unix {
    # Fix issue with c++ version used to compile Qt in some distros (Ubuntu) with Qt <= 5.12.
    # Need to check the version of c++ used with distros providing Qt > 12
    equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 13):QMAKE_CXXFLAGS += "-fno-sized-deallocation"
} else:macx {
    # Just in case...
}



# OpenImageIO

# unix:!macx: LIBS += -lOpenImageIO \
#    -lOpenImageIO_Util

#INCLUDEPATH += /usr/include/OpenImageIO
#DEPENDPATH += /usr/include/OpenImageIO
