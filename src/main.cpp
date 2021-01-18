#include <QBitmap>
#include <QSettings>
#include <QStringList>
#include <QtDebug>

#ifdef Q_OS_WIN
#include "windows.h"
#endif

#include "global.h"

#include "duqf-app/app-utils.h"
#include "UI/mainwindow.h"

// Process the CLI arguments
bool processArgs(int argc, char *argv[])
{
    bool presets = false;

    for (int i = 1; i < argc; i++)
    {
        QString arg = argv[i];
        if (arg.toLower() == "-h:presets" || arg.toLower() == "--help:presets" ) presets = true;
    }

    if (presets)
    {
        qInfo() << "Default Preset:";
        qInfo().noquote() << presetManager->defaultPreset().name();
        qInfo() << "";
        qInfo() << "Available presets:";
        foreach(Preset p, presetManager->presets())
        {
            qInfo().noquote() << p.name();
        }
    }

    return presets;
}

void initSettings(DuSplashScreen *s)
{
    s->newMessage("Reading settings...");
    QSettings settings;
    QString prevVersionStr = settings.value("version", "").toString();
    QVersionNumber prevVersion = QVersionNumber::fromString( prevVersionStr );
    if ( prevVersion.majorVersion() != VERSION_MAJOR || prevVersion.minorVersion() != VERSION_MINOR )
    {
        settings.clear();
    }
    if (prevVersionStr != STR_VERSION)
    {
        settings.setValue("version", STR_VERSION);
        settings.setValue("ffmpeg/version","");
    }
}

void initFFmpeg(DuSplashScreen *s)
{
    s->newMessage("Loading FFmpeg...");
    QObject::connect(ffmpeg,&FFmpeg::newLog,s,&DuSplashScreen::newMessage);
    QObject::connect(ffmpeg,&FFmpeg::progressMax,s,&DuSplashScreen::progressMax);
    QObject::connect(ffmpeg,&FFmpeg::progress,s,&DuSplashScreen::progress);
    s->newMessage("Initializing FFmpeg...");
    ffmpeg->init();
}

void buildUI(QStringList args, DuSplashScreen *s)
{
    s->newMessage("Building UI");
    MainWindow *w = new MainWindow( args );
#ifndef Q_OS_LINUX
    FrameLessWindow f(w);
#endif
    w->show();

    //hide splash when finished
    s->finish(w);
}

int main(int argc, char *argv[])
{
    DuApplication a(argc, argv);

    //load presets
    presetManager->load();
    //process CLI arguments
    QStringList examples;
    examples << "Usage: DuME [options] inputFile1 [[options] inputFile2 ... [options] inputFileN]";
    examples << "";
    examples << "Example: add a PNG sequence and set its framerate:";
    examples << "    DuME -framerate 29.97 \"image sequence_0001.png\"";
    examples << "";
    examples << "Example: add a PNG sequence and set its framerate along with a WAV sound file, select a preset, and autostart the transcoding process::";
    examples << "    DuME -framerate 24 \"image sequence_0001.png\" \"an awesome sound.wav\" -preset \"MP4 - Standard\" -autostart";
    QStringList helpStrings;
    helpStrings << "    --minimize / -m         Minimizes the window as soon as it is ready.";
    helpStrings << "    --help:presets / -h:p   Prints the list of available presets";
    helpStrings << "";
    helpStrings << "Global Input Options";
    helpStrings << "    --color-profile profile     The input color profile. One of: srgb, bt709, bt2020_10, bt2020_12";
    helpStrings << "";
    helpStrings << "Image Sequence and After Effects Input Options";
    helpStrings << "    --framerate fps / -f fps     The input framerate";
    helpStrings << "";
    helpStrings << "After Effects Input Options";
    helpStrings << "    --comp composition / -c composition     The name of the composition to render";
    helpStrings << "    --rq-item item                          The index of the After EFfects render queue item to render";
    helpStrings << "    --use-queue                             Just launch the After Effects render queue";
    helpStrings << "    --aerender aerenderpath                 Explicitly set the After Effects renderer to use";
    helpStrings << "";
    helpStrings << "Global Output Options";
    helpStrings << "    --output file / -o file     The name of the output file";
    helpStrings << "    --preset name / -p name     The name of the preset to use. Use `-help:presets` to get the list of available presets. You can also provide any preset filename to use a custom preset not available in DuME.";
    helpStrings << "    --autostart                 Autostart the transcoding process";
    helpStrings << "    --autoquit                  If `autostart` is set, automatically closes DuME once the transcoding process is finished";
    if ( duqf_processArgs(argc, argv, examples, helpStrings) ) return 0;
    if ( processArgs(argc, argv) ) return 0;
    //show splashscreen
    a.showSplashScreen();
    // init settings
    initSettings(a.splashScreen());
    //load FFmpeg
    initFFmpeg(a.splashScreen());
    //prep cache
    cacheManager->init();
    //build UI and show
    buildUI(a.arguments(), a.splashScreen());

    return a.exec();
}
