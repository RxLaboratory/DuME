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

bool OcioLutBakerInfo::setBinary(QString path)
{
    QSettings settings;
    if (path == "")
    {
        path = settings.value("ociobakelut/path", "ociobakelut").toString();

        //check
        bool found = false;
        if ( AbstractRendererInfo::setBinary(path) )
        {
            if (runCommand( "", 1000))
                found = _output.startsWith("ociobakelut");
        }


        if (!found)
        {
#ifdef Q_OS_LINUX
            //First, look for a dume static build in the app folder
            path = QCoreApplication::applicationDirPath() + "/dume-ociobakelut";
            //IF not found, try with a standard name
            if (!QFileInfo::exists(path)) path = QCoreApplication::applicationDirPath() + "/ociobakelut";
            //IF not found, try with a system command
            if (!QFileInfo::exists(path)) path = "ociobakelut";
#endif
#ifdef Q_OS_WIN
            path = QCoreApplication::applicationDirPath() + "/ociobakelut.exe";
            if (!QFileInfo::exists(path)) path = QCoreApplication::applicationDirPath() + "/ext/sociobakelut.exe";
#endif
#ifdef Q_OS_MAC
            path = QCoreApplication::applicationDirPath() + "/ociobakelut";
            if (!QFileInfo::exists(path)) path = "ociobakelut";
#endif
        }
    }

    if ( AbstractRendererInfo::setBinary(path) )
    {
        runCommand( "", 1000);
        if (_output.length() < 10)
        {
            _valid = false;
            return false;
        }
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


