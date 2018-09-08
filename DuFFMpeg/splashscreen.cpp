#include "splashscreen.h"

SplashScreen::SplashScreen(const QPixmap &pixmap, Qt::WindowFlags f) : QSplashScreen (pixmap,f)
{

}

void SplashScreen::newMessage(QString message)
{
    QString m = "DuFFmpeg v";
    m += APPVERSION;
    m += "\n";
    showMessage(m + message, Qt::AlignBottom, Qt::black);
}
