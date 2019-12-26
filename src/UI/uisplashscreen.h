#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QObject>
#include <QSplashScreen>

#include "utils.cpp"

class UISplashScreen : public QSplashScreen
{
public:
    UISplashScreen(const QPixmap &pixmap = QPixmap(), Qt::WindowFlags f = Qt::WindowFlags());

public slots:
    void newMessage(QString message, LogUtils::LogType lt = LogUtils::Information);
};

#endif // SPLASHSCREEN_H
