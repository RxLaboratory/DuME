#ifndef DUQFLOGGER_H
#define DUQFLOGGER_H

#include <QObject>
#include <QTime>
#include <QColor>
#include "utils.h"

class DuQFLog
{
public:
    /**
     * @brief The LogType enum Log level for printing the debug log
     */
    enum LogType { Data = -1,
                   Debug = 0,
                   Information = 1,
                   Warning = 2,
                   Critical = 3,
                   Fatal = 4 };

    explicit DuQFLog(QString message, LogType type = Information, QString component = "");

    QString message() const;
    LogType type() const;
    QString component() const;

    QString timeString() const;
    QString typeString() const;
    QColor color(bool darkBG = true);

private:
    QString _m;
    LogType _t;
    QString _c;
    QString _time;
};

class DuQFLoggerObject: public QObject
{
    Q_OBJECT
public:
    explicit DuQFLoggerObject(QString component = "", QObject *parent = nullptr);
signals:
    void newLog(DuQFLog message);
protected:
    void log(QString message, DuQFLog::LogType type = DuQFLog::Information);
private:
    QString _c;
};

class DuQFLogger : public QObject
{
    Q_OBJECT
public:
    static DuQFLogger *instance();

    QList<DuQFLog> logs();
    QList<DuQFLog> logs(DuQFLog::LogType minLevel);
    void clear();

    void tie(DuQFLoggerObject *o);

public slots:
    void log(QString message);
    void log(DuQFLog message);

signals:
    void newLog(DuQFLog message);

protected:
    static DuQFLogger *_instance;

private:
    // Singleton, private constructor
    explicit DuQFLogger(QObject *parent = nullptr);
    QList<DuQFLog> _logs;
};

#endif // DUQFLOGGER_H
