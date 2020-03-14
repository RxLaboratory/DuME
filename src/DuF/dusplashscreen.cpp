#include "dusplashscreen.h"

DuSplashScreen::DuSplashScreen(const QPixmap &pixmap, QString version, Qt::WindowFlags f) : QSplashScreen(pixmap, f)
{
    //add a progress bar
    _progressBar = new QProgressBar(this);
    _progressBar->setMinimumWidth( pixmap.width() );
    _progressBar->setMaximumWidth( pixmap.width() );
    _progressBar->move( 0, pixmap.height() - 30);
    _progressBar->setAlignment(Qt::AlignVCenter);
    //add a label for the version
    _versionLabel = new QLabel(version, this);
    _versionLabel->setMinimumWidth( pixmap.width() );
    _versionLabel->setMaximumWidth( pixmap.width() );
    _versionLabel->move( 0, pixmap.height() - 45);
}

void DuSplashScreen::newMessage(QString message, LogUtils::LogType lt)
{
    if (!this->isVisible()) return;
    if (lt == LogUtils::Debug) return;
    _progressBar->setFormat( "%p% - " + message );
#ifdef QT_DEBUG
    qDebug().noquote() << message;
#endif
    repaint();
}

void DuSplashScreen::progressMax(int max)
{
    if (!this->isVisible()) return;
    _progressBar->setMaximum( max );
}

void DuSplashScreen::progress(int val)
{
    if (!this->isVisible()) return;
    _progressBar->setValue(val);
    repaint();
}
