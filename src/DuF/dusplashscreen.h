#ifndef DUSPLASHSCREEN_H
#define DUSPLASHSCREEN_H

#include <QSplashScreen>
#include <QProgressBar>
#include <QLabel>
#include <QtDebug>

#include "utils.h"

class DuSplashScreen : public QSplashScreen
{
public:
    DuSplashScreen(const QPixmap &pixmap = QPixmap(), QString version = "", Qt::WindowFlags f = Qt::WindowFlags());

public slots:
    void newMessage(QString message, LogUtils::LogType lt = LogUtils::Information);
    void progressMax(int max);
    void progress(int val);

private:
    QProgressBar *_progressBar;
    QLabel *_versionLabel;
};

#endif // DUSPLASHSCREEN_H
