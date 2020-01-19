#include "UI/mainwindow.h"
#include "UI/framelesswindow.h"

#include <QApplication>
#include <QBitmap>
#include <QSettings>
#include <QStringList>
#include <QtDebug>

#include "FFmpeg/ffmpeg.h"
#include "UI/rainboxui.h"
#include "version.h"

//global: app version
AppVersion DuMEVersion(VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,VERSION_SUFFIX);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString currentVersionStr = DuMEVersion.getString();

    //set style
    RainboxUI::updateCSS(":/styles/default", "dume");

    //load settings
    QCoreApplication::setOrganizationName(STR_COMPANYNAME);
    QCoreApplication::setOrganizationDomain(STR_COMPANYDOMAIN);
    QCoreApplication::setApplicationName(STR_PRODUCTNAME);
    QCoreApplication::setApplicationVersion(currentVersionStr);

    //create splash screen
    QString v = "DuME v";
    QPixmap pixmap(":/images/splash");
    UISplashScreen splash(pixmap, v+currentVersionStr);

    //set app icon
    qApp->setWindowIcon(QIcon(":/icons/app"));

    //show
    splash.show();


    QSettings settings;
    QString prevVersionStr = settings.value("version", "").toString();
    QVersionNumber prevVersion = QVersionNumber::fromString( prevVersionStr );
    if ( prevVersion.majorVersion() != DuMEVersion.majorVersion() || prevVersion.minorVersion() != DuMEVersion.minorVersion() )
    {
        settings.clear(); 
    }
    if (prevVersionStr != currentVersionStr)
    {
        settings.setValue("version", currentVersionStr);
        settings.setValue("ffmpeg/version","");
    }


    // UNCOMMENT ONLY TO FORCE FFMPEG RELOAD. COMMENT THIS LINE OUT BEFORE COMMITTING CHANGES!
    //settings.setValue("ffmpeg/version","");


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
    MainWindow *w = new MainWindow( argc, argv, ffmpeg );
#ifndef Q_OS_LINUX
    FrameLessWindow f(w);
#endif
    w->show();

    //hide splash when finished
    splash.finish(w);

    //end appli
    return a.exec();
}
