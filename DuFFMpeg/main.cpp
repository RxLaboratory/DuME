#include "mainwindow.h"
#include "frameless.h"
#include <QApplication>
#include <QBitmap>
#include <QSettings>
#include <QStringList>

#include "ffmpeg.h"
#include "aerender.h"
#include "splashscreen.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //create splash screen
    QPixmap pixmap(":/images/splash");
    SplashScreen splash(pixmap);
    //add a progress bar?
    //QProgressBar progress(&splash);
    //show
    splash.show();

    //load settings
    splash.newMessage("Loading settings...");
    QCoreApplication::setOrganizationName("Rainbox Laboratory");
    QCoreApplication::setOrganizationDomain("rainboxlab.org");
    QCoreApplication::setApplicationName("DuFFmpeg");
    QCoreApplication::setApplicationVersion(APPVERSION);

    QSettings settings;
    if (settings.value("version").toString() != APPVERSION)
    {
        settings.setValue("version",APPVERSION);
        settings.setValue("ffmpeg/version","");
    }


#ifdef QT_DEBUG
    //settings.setValue("ffmpeg/version","");
#endif

    //load FFmpeg
    splash.newMessage("Initializing FFmpeg...");
    FFmpeg *ffmpeg = new FFmpeg();
    QObject::connect(ffmpeg,&FFmpeg::debugInfo,&splash,&SplashScreen::newMessage);
    ffmpeg->init();

    //build UI and show
    splash.newMessage("Building UI");
    MainWindow *w = new MainWindow(ffmpeg);
#ifndef Q_OS_LINUX
    FrameLess f(w);
#endif
    w->show();


    //hide splash when finished
    splash.finish(w);

    //end appli
    return a.exec();
}
