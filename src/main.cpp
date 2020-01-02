#include "UI/uimainwindow.h"
#include "UI/uiframeless.h"

#include <QApplication>
#include <QBitmap>
#include <QSettings>
#include <QStringList>
#include <QtDebug>

#include "FFmpeg/ffmpeg.h"
#include "UI/uisplashscreen.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //set style
    UIRainbox::updateCSS(":/styles/default", "dume");

    //load settings
    QCoreApplication::setOrganizationName("Rainbox Laboratory");
    QCoreApplication::setOrganizationDomain("rainboxlab.org");
    QCoreApplication::setApplicationName("DuME");
    QCoreApplication::setApplicationVersion(APPVERSION);

    //create splash screen
    QString v = "DuME v";
    QPixmap pixmap(":/images/splash");
    UISplashScreen splash(pixmap, v+APPVERSION);

    //show
    splash.show();


    QSettings settings;
    if (settings.value("version").toString() != APPVERSION)
    {
        settings.setValue("version",APPVERSION);
        settings.setValue("ffmpeg/version","");
    }


    // UNCOMMENT ONLY TO FORCE FFMPEG RELOAD. COMMENT THIS LINE OUT BEFORE COMMITTING CHANGES!
     settings.setValue("ffmpeg/version","");


    //load FFmpeg
    splash.newMessage("Loading FFmpeg...");
    FFmpeg *ffmpeg = new FFmpeg();
    QObject::connect(ffmpeg,&FFmpeg::newLog,&splash,&UISplashScreen::newMessage);
    QObject::connect(ffmpeg,&FFmpeg::progressMax,&splash,&UISplashScreen::progressMax);
    QObject::connect(ffmpeg,&FFmpeg::progress,&splash,&UISplashScreen::progress);
    splash.newMessage("Initializing FFmpeg...");
    ffmpeg->init();

    //build UI and show
    splash.newMessage("Building UI");
    UIMainWindow *w = new UIMainWindow( ffmpeg );
#ifndef Q_OS_LINUX
    UIFrameLess f(w);
#endif
    w->show();

    //hide splash when finished
    splash.finish(w);

    //end appli
    return a.exec();
}
