#include "ociolutbakerinfo.h"

OcioLutBakerInfo *OcioLutBakerInfo::_instance = nullptr;

OcioLutBakerInfo *OcioLutBakerInfo::instance()
{
    if (!_instance) _instance = new OcioLutBakerInfo;
    return _instance;
}

OcioLutBakerInfo::OcioLutBakerInfo(QObject *parent) :
   AbstractRendererInfo(parent)
{
    setBinary();
}

bool OcioLutBakerInfo::checkBinaryPath(QString path)
{
    if ( AbstractRendererInfo::setBinary(path) )
    {
        qDebug() << runCommand( "", 1000);
        qDebug() << "========";
        qDebug() << _output;
        if ( _output.startsWith("ociobakelut") ) return true;
    }
    return false;
}

bool OcioLutBakerInfo::setBinary(QString path)
{
    QSettings settings;
    if (path == "") path = settings.value("ociobakelut/path", "ociobakelut").toString();

    bool found = checkBinaryPath(path);

#ifdef Q_OS_LINUX
            //First, look for a dume static build in the app folder
            if ( !found ) path = QCoreApplication::applicationDirPath() + "/dume-ociobakelut";
            found = checkBinaryPath(path);
            //IF not found, try with a standard name
            if ( !found ) path = QCoreApplication::applicationDirPath() + "/ociobakelut";
            found = checkBinaryPath(path);
            //IF not found, try with a system command
            if ( !found ) path = "ociobakelut";
            found = checkBinaryPath(path);
#endif
#ifdef Q_OS_WIN
            if ( !found ) path = QCoreApplication::applicationDirPath() + "/ociobakelut.exe";
            found = checkBinaryPath(path);
            if ( !found ) path = QCoreApplication::applicationDirPath() + "/ext/ociobakelut.exe";
            found = checkBinaryPath(path);
#endif
#ifdef Q_OS_MAC
            if ( !found ) path = QCoreApplication::applicationDirPath() + "/ociobakelut";
            found = checkBinaryPath(path);
            // Try with homebrew
            if ( !found ) path = "/opt/homebrew/Cellar/opencolorio/2.0.0/bin/ociobakelut";
            found = checkBinaryPath(path);
            if ( !found ) path = "/usr/local/Cellar/opencolorio/2.0.0/bin/ociobakelut";
            found = checkBinaryPath(path);
#endif

    if ( found )
    {
        //save
        settings.setValue("ociobakelut/path", path);
        emit binaryChanged( path );
        qDebug() << "New ociobakelut path correctly set: " + path;
        _valid = true;
        return true;
    }

    _valid = false;
    return _valid;
}


