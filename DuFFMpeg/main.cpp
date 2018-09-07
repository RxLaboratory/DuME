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
    QString message = "DuFFmpeg v";
    message += APPVERSION;
    message += " | ";
    splash.showMessage(message + "Loading settings...");
    QCoreApplication::setOrganizationName("Duduf");
    QCoreApplication::setOrganizationDomain("duduf.com");
    QCoreApplication::setApplicationName("DuFFmpeg");
    QCoreApplication::setApplicationVersion(APPVERSION);
    QSettings settings;

    //load FFmpeg
    splash.showMessage(message + "Initializing FFmpeg...");
    //TODO auto find ffmpeg if no settings or path invalid
    //then save to settings
    FFmpeg *ffmpeg = new FFmpeg(settings.value("ffmpeg/path","ffmpeg.exe").toString());
    splash.showMessage(message + "Loading After Effects Renderer");
    //TODO autodetect
    ffmpeg->setAERenderFileName(settings.value("aerender/path","C:\\Program Files\\Adobe\\Adobe After Effects CC 2018\\Support Files\\aerender.exe").toString());

    //build UI and show
    splash.showMessage(message + "Building UI");
    MainWindow *w = new MainWindow(ffmpeg);
    FrameLess f(w);
    w->show();
    //hide splash when finished
    splash.finish(w);

    //end appli
    return a.exec();
}
