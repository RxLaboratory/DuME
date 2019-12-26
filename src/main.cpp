#include "UI/uimainwindow.h"
#include "UI/uiframeless.h"

#include <QApplication>
#include <QBitmap>
#include <QSettings>
#include <QStringList>

#include "FFmpeg/ffmpeg.h"
#include "UI/uisplashscreen.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //create splash screen
    QPixmap pixmap(":/images/splash");
    UISplashScreen splash(pixmap);
    //add a progress bar?
    //QProgressBar progress(&splash);
    //show
    splash.show();

    //load settings
    splash.newMessage("Loading settings...");
    QCoreApplication::setOrganizationName("Rainbox Laboratory");
    QCoreApplication::setOrganizationDomain("rainboxlab.org");
    QCoreApplication::setApplicationName("DuME");
    QCoreApplication::setApplicationVersion(APPVERSION);

    QSettings settings;
    if (settings.value("version").toString() != APPVERSION)
    {
        settings.setValue("version",APPVERSION);
        settings.setValue("ffmpeg/version","");
    }

#ifdef FORCE_FFMPEG_RELOAD
    settings.setValue("ffmpeg/version","");
#endif

    //load FFmpeg
    splash.newMessage("Loading FFmpeg...");
    FFmpeg *ffmpeg = new FFmpeg();
    QObject::connect(ffmpeg,&FFmpeg::newLog,&splash,&UISplashScreen::newMessage);
    splash.newMessage("Initializing FFmpeg...");
    ffmpeg->init();

    //build UI and show
    splash.newMessage("Building UI");
    UIMainWindow *w = new UIMainWindow( ffmpeg );
#ifndef Q_OS_LINUX
    FrameLess f(w);
#endif
    w->show();

    //hide splash when finished
    splash.finish(w);

    //end appli
    return a.exec();
}
