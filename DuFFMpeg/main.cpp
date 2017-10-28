#include "mainwindow.h"
#include "frameless.h"
#include <QApplication>
#include <QSplashScreen>
#include <QBitmap>
#include <QSettings>

#include "ffmpeg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //create splash screen
    QPixmap pixmap(":/images/splash");
    QSplashScreen splash(pixmap);
    //add a progress bar?
    //QProgressBar progress(&splash);
    //show
    splash.show();

    //load settings
    splash.showMessage("Loading settings...");
    QCoreApplication::setOrganizationName("Duduf");
    QCoreApplication::setOrganizationDomain("duduf.com");
    QCoreApplication::setApplicationName("DuFFmpeg");
    QCoreApplication::setApplicationVersion(APPVERSION);
    QSettings settings;

    //load FFmpeg
    splash.showMessage("Initializing FFmpeg...");
    //TODO auto find ffmpeg if no settings or path invalid
    //then save to settings
    FFmpeg *ffmpeg = new FFmpeg(settings.value("ffmpeg/path","ffmpeg.exe").toString());


    //build UI and show
    splash.showMessage("Building UI");
    MainWindow *w = new MainWindow(ffmpeg);
    FrameLess f(w);
    w->show();
    //hide splash when finished
    splash.finish(w);

    //end appli
    return a.exec();
}
