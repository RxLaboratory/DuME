#include "uisplashscreen.h"

UISplashScreen::UISplashScreen(const QPixmap &pixmap, Qt::WindowFlags f) : QSplashScreen (pixmap,f)
{

}

void UISplashScreen::newMessage(QString message)
{
    QString m = "DuFFmpeg v";
    m += APPVERSION;
    m += "\n";
    showMessage(m + message, Qt::AlignBottom, Qt::black);
}
