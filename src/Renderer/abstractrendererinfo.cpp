#include "abstractrendererinfo.h"

AbstractRendererInfo::AbstractRendererInfo(QObject *parent) : QObject(parent)
{

}

QString AbstractRendererInfo::binary() const
{
    return _binary;
}

bool AbstractRendererInfo::setBinary(QString binary)
{
    bool fileExists = true;

#ifdef Q_OS_WIN
    fileExists = QFile(path).exists();
#endif

    if(fileExists)
    {
        _binary = binary;
        emit binaryChanged( binary );
        return true;
    }
    else
    {
        emit newLog("Renderer executable binary not found.", LogUtils::Warning );
        return false;
    }
}

void AbstractRendererInfo::log(QString l, LogUtils::LogType lt)
{
    emit newLog( l, lt );
}
