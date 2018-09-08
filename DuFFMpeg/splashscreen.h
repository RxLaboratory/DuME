#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QObject>
#include <QSplashScreen>

class SplashScreen : public QSplashScreen
{
public:
    SplashScreen(const QPixmap &pixmap = QPixmap(), Qt::WindowFlags f = Qt::WindowFlags());

public slots:
    void newMessage(QString message);
};

#endif // SPLASHSCREEN_H
