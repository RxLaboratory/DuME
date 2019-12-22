#include "aerenderer.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

AERenderer::AERenderer(QObject *parent) : QObject(parent)
{
    init();
}

QList<AERenderProcess *> AERenderer::versions() const
{
    return _versions;
}

bool aeVersionSorter(AERenderProcess *v1, AERenderProcess *v2)
{
    if (v1->version() < v2->version()) return true;
    if (v1->version() > v2->version()) return false;

    if (v1->name().toLower() < v2->name().toLower()) return true;
    if (v1->name().toLower() > v2->name().toLower()) return false;

    return true;
}

void AERenderer::init()
{
    emit newLog("Initializing the After Effects renderer");

    //get aerender paths
#ifdef Q_OS_WIN
    //Search all drives
    QFileInfoList drives = QDir::drives();
    foreach(QFileInfo drive, drives)
    {
        findAeVersions(drive.absoluteFilePath() + "Program Files\\Adobe\\");
        findAeVersions(drive.absoluteFilePath() + "Program Files (x86)\\Adobe\\");
    }

#endif
#ifdef Q_OS_MAC
    findAeVersions("/Applications");
#endif

    //sort
    std::sort(_versions.begin(),_versions.end(),aeVersionSorter);

    setCurrentAERenderProcess();
}

void AERenderer::setCurrentAERenderProcess( QString name )
{
    emit newLog("Selecting the After Effects renderer");

    QSettings settings;

    currentAERenderProcess = nullptr;

#if INIT_AE //used when developping to skip Ae loading

    if ( name == "Custom" )
    {
        // Create and add the custom process to the list
        AERenderProcess *ae = new AERenderProcess( settings.value("aerender/path","").toString() );
        if ( ae->isValid() )
        {
            _versions << ae;
            currentAERenderProcess = ae;
            name = ae->name();
        }
        else
        {
            emit newError("Invalid After Effects custom renderer at: " + settings.value("aerender/path","").toString());
            //revert to use latest
            settings.setValue( "aerender/useLatest", true );
            name = _versions.last()->name();
        }
    }

    for (int i = 0; i < _versions.count(); i++)
    {
        if (_versions[i]->name() == name)
        {
            currentAERenderProcess = _versions[i];

            settings.setValue( "aerender/version", _versions[i]->name() );
            settings.setValue("aerender/path", _versions[i]->path() );

            emit newLog("After Effects set to renderer: " + _versions[i]->name() );
            break;
        }
    }

#endif
}

void AERenderer::start(QStringList arguments, QStringList audioArguments, int numThreads, bool useQueue )
{
    bool audio = audioArguments.count() > 0;
    if (audio && numThreads >= QThread::idealThreadCount() && !useQueue ) numThreads--;

    //video
    for (int i = 0; i < numThreads; i++ )
    {
        launchRenderProcess( arguments );
    }
    //audio
    if (audio)
    {
        launchRenderProcess( audioArguments );
    }
}

void AERenderer::launchRenderProcess(QStringList args)
{
    //create process
    QProcess *aerender = new QProcess(this);
    connect(aerender,SIGNAL(readyReadStandardError()),this,SLOT(stdError()));
    connect(aerender,SIGNAL(readyReadStandardOutput()),this,SLOT(stdOutput()));
    connect(aerender,SIGNAL(started()),this,SLOT(started()));
    connect(aerender,SIGNAL(finished(int)),this,SLOT(finished()));
    connect(aerender,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurred(QProcess::ProcessError)));

    //launch
    aerender->setProgram(currentAERenderProcess->path());
    aerender->setArguments(args);
    aerender->start(QIODevice::ReadWrite);

    //TODO check processor affinity?

    _renderProcesses << aerender;
}

void AERenderer::setCurrentAERenderProcess( )
{
    QSettings settings;

    QString name = "Custom";

    if ( settings.value("aerender/useLatest",true).toBool() )
    {
        name = _versions.last()->name();
    }
    else
    {
        name = settings.value("aerender/version","Custom").toString();
    }

    setCurrentAERenderProcess( name );
}

void AERenderer::findAeVersions(QString dir)
{
    QStringList filters("*After Effects *");

    QDir adobeDir(dir);

    adobeDir.setNameFilters(filters);
    adobeDir.setFilter(QDir::Dirs);

    QFileInfoList versionPaths = adobeDir.entryInfoList();

    foreach(QFileInfo path,versionPaths)
    {
#ifdef Q_OS_WIN
        QString aerenderFile(path.absoluteFilePath() + "/Support Files/aerender.exe");
#endif
#ifdef Q_OS_MAC
        QString aerenderFile(path.absoluteFilePath() + "/aerender");
#endif
#ifdef Q_OS_LINUX
        QString aerenderFile("");
#endif
        AERenderProcess *ae = new AERenderProcess(aerenderFile);
        if (ae->isValid()) _versions << ae;
    }
}

void AERenderer::threadFinished()
{
    //check which processes have finished
    QList<int> finishedProcesses;
    QString debugFinishedProcesses = "Finished threads: ";
    for(int i = 0; i < _renderProcesses.count(); i++)
    {
        if( _renderProcesses[i]->state() == QProcess::NotRunning )
        {
            finishedProcesses << i;
            if (i > 0) debugFinishedProcesses += ", ";
            debugFinishedProcesses += QString::number(i);
        }
    }

    newLog( debugFinishedProcesses );

    //if all processes have finished
    if (finishedProcesses.count() == _renderProcesses.count())
    {
        //remove all processes
        while( _renderProcesses.count() > 0 )
        {
            QProcess *aerender = _renderProcesses.takeLast();
            aerender->deleteLater();
        }

        emit finished();
    }
}
