#ifndef ABSTRACTRENDERERINFO_H
#define ABSTRACTRENDERERINFO_H

#include <QObject>

#include "utils.h"

class AbstractRendererInfo : public QObject
{
    Q_OBJECT
public:
    explicit AbstractRendererInfo(QObject *parent = nullptr);

    QString binary() const;

signals:
    void newLog( QString, LogUtils::LogType lt = LogUtils::Information );
    void binaryChanged( QString binary );

public slots:
    bool setBinary( QString binary );

protected slots:
    void log( QString l, LogUtils::LogType lt = LogUtils::Information );

private:
    QString _binary;

};

#endif // ABSTRACTRENDERERINFO_H
