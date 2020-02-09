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

    //if DuME is launched with the -h or -help option, just print some informations
    for (int i = 1; i < argc; i++)
    {
        QString arg = argv[i];
        if (arg == "-h" || arg == "-help" )
        {
            qInfo().noquote() << "DuME - The Duduf Media Encoder version " + DuMEVersion.getString() + " " + STR_LEGALCOPYRIGHT;
            qInfo() << "This program comes with ABSOLUTELY NO WARRANTY;";
            qInfo() << "This is free software, and you are welcome to redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.";
            qInfo() << "";
            qInfo() << "A fast, versatile, and easy-to-use Media Encoder.";
            qInfo() << "Usage: DuME [options] inputFile1 [[options] inputFile2 ... [options] inputFileN]";
            qInfo() << "";
            qInfo() << "Example: add a PNG sequence and set its framerate:";
            qInfo() << "    DuME -framerate 29.97 \"image sequence_0001.png\"";
            qInfo() << "";
            qInfo() << "Example: add a PNG sequence and set its framerate along with a WAV sound file:";
            qInfo() << "    DuME -framerate 24 \"image sequence_0001.png\" \"an awesome sound.wav\"";
            qInfo() << "";
            qInfo() << "Getting help";
            qInfo() << "    -h / -help    Print basic options without launching DuME";
            qInfo() << "    See the documentation at https://dume-docs.rainboxlab.org for detailed descriptions of the options";
            qInfo() << "";
            qInfo() << "Global Input Options";
            qInfo() << "    -colorProfile profile    The input color profile. One of: srgb, bt709, bt2020_10, bt2020_12";
            qInfo() << "";
            qInfo() << "Image Sequence and After Effects options";
            qInfo() << "    -framerate fps    The input framerate";
            qInfo() << "    -fps fps          The input framerate";
            qInfo() << "";
            qInfo() << "After Effects options";
            qInfo() << "    -comp composition    The name of the composition to render";
            qInfo() << "    -rqItem item         The index of the After EFfects render queue item to render";
            qInfo() << "    -useQueue            Just launch the After Effects render queue";

            return 0;
        }
    }

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
