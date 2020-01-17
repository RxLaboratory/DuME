#include "rainboxui.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

RainboxUI::RainboxUI()
{

}

void RainboxUI::updateCSS( QString cssFileName, QString appName)
{
    qApp->setStyleSheet("");
    QStringList cssFiles(cssFileName);
    //check if there's a dume file to include
    QFileInfo cssFileInfo(cssFileName);
    QString includeName = cssFileInfo.completeBaseName() + "-" + appName;
    QString includePath = cssFileInfo.path() + "/" + includeName + ".css";
    QFile includeFile(includePath);
    includePath = cssFileInfo.path() + "/" + includeName;
    if (!includeFile.exists()) includeFile.setFileName(includePath);
    if (includeFile.exists())
    {
        cssFiles << includePath;
    }
    QString css = RainboxUI::loadCSS(cssFiles);
    qApp->setStyleSheet(css);
}

QString RainboxUI::loadCSS(QString cssFileName)
{
    return loadCSS(QStringList(cssFileName));
}

QString RainboxUI::loadCSS(QStringList cssFileNames)
{
    QString css = "";

    //Build a single CSS with the files
    foreach(QString file,cssFileNames)
    {
        QFile cssFile(file);
        if (cssFile.exists())
        {
            if (cssFile.open(QFile::ReadOnly))
            {
                css += QString(cssFile.readAll());
                cssFile.close();
            }
        }
    }

    //replace variables
    //find values
    QFileInfo cssInfo(cssFileNames[0]);
    QString baseName = cssInfo.path() + "/" + cssInfo.completeBaseName();
    QFile valuesFile(baseName + "-values.rui");
    if (!valuesFile.exists()) valuesFile.setFileName(baseName + "-values");
    if (!valuesFile.exists()) valuesFile.setFileName(baseName + "-values.txt");
    if (!valuesFile.exists()) valuesFile.setFileName(baseName + ".val");
    if (!valuesFile.exists()) valuesFile.setFileName(baseName + ".txt");
    if (!valuesFile.exists()) valuesFile.setFileName(baseName + ".rui");
    if (valuesFile.exists())
    {
        if (valuesFile.open(QFile::ReadOnly))
        {
            css += "\n";
            //read lines
            while(!valuesFile.atEnd())
            {
                QString line = valuesFile.readLine();

                //get value and name
                QRegularExpression re("(@\\S+) *= *(\\S+)");
                QRegularExpressionMatch match = re.match(line);
                if (match.hasMatch())
                {
                    QString name = match.captured(1);
                    QString value = match.captured(2);
                    css.replace(name,value);
                }
            }
        }
    }

#ifdef QT_DEBUG
    qDebug() << "Rainbox UI: Loading CSS";
    //qDebug() << css;
#endif

    return css;
}

void RainboxUI::setFont(QString family)
{
    //add fonts
    QFontDatabase::addApplicationFont(":/fonts/calibri");
    QFontDatabase::addApplicationFont(":/fonts/calibri_bold");
    QFontDatabase::addApplicationFont(":/fonts/calibri_italic");
    QFontDatabase::addApplicationFont(":/fonts/calibri_light");
    QFontDatabase::addApplicationFont(":/fonts/calibri_light_italic");
    QFontDatabase::addApplicationFont(":/fonts/calibri_bold_italic");

    qApp->setFont(QFont(family,10,500),"QWidget");
}

UISplashScreen::UISplashScreen(const QPixmap &pixmap, QString version, Qt::WindowFlags f) : QSplashScreen (pixmap,f)
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

void UISplashScreen::newMessage(QString message, LogUtils::LogType lt)
{
    if (lt == LogUtils::Debug) return;
    _progressBar->setFormat( "%p% - " + message );
#ifdef QT_DEBUG
    qDebug().noquote() << message;
#endif
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
