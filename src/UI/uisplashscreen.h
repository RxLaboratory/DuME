#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QObject>
#include <QSplashScreen>
#include <QProgressBar>
#include <QLabel>
#include <QtDebug>

#include "utils.cpp"

class UISplashScreen : public QSplashScreen
{
public:
    UISplashScreen(const QPixmap &pixmap = QPixmap(), QString version = "", Qt::WindowFlags f = Qt::WindowFlags());

public slots:
    void newMessage(QString message, LogUtils::LogType lt = LogUtils::Information);
    void progressMax(int max);
    void progress(int val);

private:
    QProgressBar *_progressBar;
    QLabel *_versionLabel;
};

#endif // SPLASHSCREEN_H
