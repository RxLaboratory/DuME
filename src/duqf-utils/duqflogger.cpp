#include "duqflogger.h"

// === Logger

DuQFLogger *DuQFLogger::_instance = nullptr;

DuQFLogger *DuQFLogger::instance()
{
    if (!_instance) _instance = new DuQFLogger();
    return _instance;
}

QList<DuQFLog> DuQFLogger::logs()
{
    return _logs;
}

QList<DuQFLog> DuQFLogger::logs(DuQFLog::LogType minLevel)
{
    QList<DuQFLog> el;
    foreach(DuQFLog l, _logs)
    {
        if (l.type() >= minLevel) el << l;
    }
    return el;
}

void DuQFLogger::clear()
{
    _logs.clear();
}

void DuQFLogger::tie(DuQFLoggerObject *o)
{
    connect(o, SIGNAL(newLog(DuQFLog)), this, SLOT(log(DuQFLog)));
}

void DuQFLogger::log(QString message)
{
    log(DuQFLog(message));
}

void DuQFLogger::log(DuQFLog message)
{
    _logs << message;

    emit newLog(message);
}

DuQFLogger::DuQFLogger(QObject *parent) : QObject(parent)
{

}

// === Log

DuQFLog::DuQFLog(QString message, DuQFLog::LogType type, QString component)
{
    _m = message;
    _t = type;
    _c = component;
    _time = QTime::currentTime().toString("[hh:mm:ss.zzz]: ");

    QString m = typeString() + " " + message;
    switch(type) {
    case Data:
#ifdef SHOWDATA
        qDebug().noquote() << m;
#endif
        break;
    case Debug:
#ifdef QT_DEBUG
        qDebug().noquote() << m;
#endif
        break;
    case Information:
        qInfo().noquote() << m;
        break;
    case Warning:
        qWarning().noquote() << m;
        break;
    case Critical:
        qCritical().noquote() << m;
        break;
    case Fatal:
        qDebug() << m;
        break;
    default:
        qDebug() << m;
    }
}

QString DuQFLog::message() const
{
    return _m;
}

DuQFLog::LogType DuQFLog::type() const
{
    return _t;
}

QString DuQFLog::component() const
{
    return _c;
}

QString DuQFLog::timeString() const
{
    return _time;
}

QColor DuQFLog::color(bool darkBG)
{
    if (darkBG)
    {
        switch(_t) {
            case DuQFLog::Debug: return QColor(109,109,109);
            case DuQFLog::Information: return QColor(157,157,157);
            case DuQFLog::Warning: return QColor(236,215,24);
            case DuQFLog::Critical: return QColor(249,105,105);
            case DuQFLog::Fatal: return QColor(213,136,241);
            default: return QColor();
        }
    }
    else
    {
        switch(_t) {
            case DuQFLog::Debug: return QColor(37,37,37);
            case DuQFLog::Information: return QColor(67,67,67);
            case DuQFLog::Warning: return QColor(16,104,142);
            case DuQFLog::Critical: return QColor(172,16,16);
            case DuQFLog::Fatal: return QColor(129,9,173);
            default: return QColor();
        }
    }
}

QString DuQFLog::typeString() const
{
    switch(_t) {
        case DuQFLog::Debug: return "(Debug) " + _c;
        case DuQFLog::Information: return _c;
        case DuQFLog::Warning: return "/!\\ Warning: " + _c;
        case DuQFLog::Critical: return " --- !!! Critical: " + _c;
        case DuQFLog::Fatal: return " === Fatal === " + _c;
        default: return _c;
    }
}

// === Logger Object

DuQFLoggerObject::DuQFLoggerObject(QString component, QObject *parent): QObject(parent)
{
    _c = component;
    DuQFLogger::instance()->tie(this);
}

void DuQFLoggerObject::log(QString message, DuQFLog::LogType type)
{
    emit newLog(DuQFLog(message, type, _c));
}
