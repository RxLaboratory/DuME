#include "uisplashscreen.h"

UISplashScreen::UISplashScreen(const QPixmap &pixmap, QString version, Qt::WindowFlags f) : QSplashScreen (pixmap,f)
{
    //add a progress bar
    _progressBar = new QProgressBar(this);
    _progressBar->setMinimumWidth( pixmap.width() );
    _progressBar->setMaximumWidth( pixmap.width() );
    _progressBar->move( 0, pixmap.height() - 30);
    //add a label for the version
    _versionLabel = new QLabel(version, this);
    _versionLabel->setMinimumWidth( pixmap.width() );
    _versionLabel->setMaximumWidth( pixmap.width() );
    _versionLabel->move( 0, pixmap.height() - 45);
}

void UISplashScreen::newMessage(QString message, LogUtils::LogType lt)
{
    if (lt == LogUtils::Debug) return;
    _progressBar->setFormat( "%p% - " + message );
    qDebug() << message;
    repaint();
}

void UISplashScreen::progressMax(int max)
{
    _progressBar->setMaximum( max );
}

void UISplashScreen::progress(int val)
{
    _progressBar->setValue(val);
    repaint();
}
