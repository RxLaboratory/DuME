#ifndef APPUTILS_H
#define APPUTILS_H

#include <QtDebug>
#include <QApplication>
#include <QIcon>
#include <QSplashScreen>
#include <QProgressBar>
#include <QLabel>
#include <QFileInfo>
#include <QFontDatabase>
#include <QRegularExpression>

#include "app-version.h"
#include "app-style.h"
#include "../duqf-utils/utils.h"

#ifdef Q_OS_WIN
#include "windows.h"
#endif

class MessageHandler
{
public:
    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        QByteArray localMsg = msg.toLocal8Bit();
        const char *file = context.file ? context.file : "";
        const char *function = context.function ? context.function : "";
        switch (type) {
        case QtDebugMsg:
    #ifdef QT_DEBUG
            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
    #endif
            break;
        case QtInfoMsg:
            fprintf(stdout, "%s\n", localMsg.constData());
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        }
    }
};

class DuSplashScreen : public QSplashScreen
{
public:
    DuSplashScreen(const QPixmap &pixmap = QPixmap(), Qt::WindowFlags f = Qt::WindowFlags()) : QSplashScreen(pixmap, f)
    {
        //add a progress bar
        _progressBar = new QProgressBar(this);
        _progressBar->setMinimumWidth( pixmap.width() );
        _progressBar->setMaximumWidth( pixmap.width() );
        _progressBar->move( 0, pixmap.height() - 30);
        _progressBar->setAlignment(Qt::AlignVCenter);
        //add a label for the version
        _versionLabel = new QLabel("v" + QString(STR_VERSION), this);
        _versionLabel->setMinimumWidth( pixmap.width() );
        _versionLabel->setMaximumWidth( pixmap.width() );
        _versionLabel->move( 0, pixmap.height() - 45);
    }

public slots:

    void newMessage(QString message, LogUtils::LogType lt = LogUtils::Information)
    {
        if (!this->isVisible()) return;
        if (lt == LogUtils::Debug) return;
        _progressBar->setFormat( "%p% - " + message );
#ifdef QT_DEBUG
    qDebug().noquote() << message;
#endif
        repaint();
    }
    void progressMax(int max)
    {
        if (!this->isVisible()) return;
        _progressBar->setMaximum( max );
    }
    void progress(int val)
    {
        if (!this->isVisible()) return;
        _progressBar->setValue(val);
        repaint();
    }

private:

    QProgressBar *_progressBar;
    QLabel *_versionLabel;
};

class DuApplication : public QApplication
{
public:
    DuApplication(int &argc, char *argv[]) : QApplication(argc, argv)
    {
#ifndef QT_DEBUG
        // handles messages from the app and redirects them to stdout (info) or stderr (debug, warning, critical, fatal)
        qInstallMessageHandler(MessageHandler::messageOutput);
#endif
        qDebug() << "Initializing application";

        //set style
        DuUI::updateCSS(":/styles/default");

        //create splash screen
        QPixmap pixmap(SPLASH_IMAGE);
        _splashScreen = new DuSplashScreen( pixmap );

        //set app icon
        qApp->setWindowIcon(QIcon(APP_ICON));

        // settings
        QCoreApplication::setOrganizationName(STR_COMPANYNAME);
        QCoreApplication::setOrganizationDomain(STR_COMPANYDOMAIN);
        QCoreApplication::setApplicationName(STR_PRODUCTNAME);
        QCoreApplication::setApplicationVersion(STR_VERSION);
    }

    DuSplashScreen *splashScreen() const
    {
        return _splashScreen;
    }

    void showSplashScreen()
    {
        qDebug() << "Displaying splash screen";
        _splashScreen->show();
    }

private:
    DuSplashScreen *_splashScreen;
};

// Process the CLI arguments
bool duqf_processArgs(int argc, char *argv[], QStringList examples = QStringList(), QStringList helpStrings = QStringList())
{
    bool nobanner = false;
    bool help = false;

    // No console without args on windows
#ifdef Q_OS_WIN
    bool hideConsole = argc == 1;
#endif

    for (int i = 1; i < argc; i++)
    {
        QString arg = argv[i];
        if ( arg.toLower() == "--no-banner" ) nobanner = true;
        if (arg.toLower() == "-h" || arg.toLower() == "--help" ) help = true;
#ifdef Q_OS_WIN
        if (arg.toLower() == "--hide-console") hideConsole = true;
#endif
    }

    if (!nobanner)
    {
        qInfo().noquote() << STR_PRODUCTNAME;
        qInfo().noquote() << "version " + QString(STR_VERSION);
        qInfo().noquote() << STR_LEGALCOPYRIGHT;
        qInfo() << "";
        qInfo() << "This program comes with ABSOLUTELY NO WARRANTY;";
        qInfo() << "This is free software, and you are welcome to redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.";
        qInfo() << "";

#ifdef QT_DEBUG
        qInfo() << "Note: this is a development build and should not be used in production.";
        qInfo() << "";
#endif
    }

    if (help)
    {
        qInfo().noquote() << STR_PRODUCTDESCRIPTION;
        foreach(QString example, examples)
        {
            qInfo().noquote() << example;
        }
        qInfo() << "General";
        qInfo() << "    -h / --help       Print basic options without launching the application";
        qInfo().noquote() << "    See the documentation at " + QString(URL_DOC) + " for detailed descriptions of the options";
        qInfo() << "    --no-banner       Hides the banner with product information and legal notice";
        foreach(QString h, helpStrings)
        {
            qInfo().noquote() << h;
        }
#ifdef Q_OS_WIN
        qInfo() << "    --hide-console    Hides the console when starting the application using command line arguments";
#endif
    }

#ifdef Q_OS_WIN
#ifndef QT_DEBUG
    if (hideConsole)
    {
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    }
#endif
#endif

    return help;
}

#endif // APPUTILS_H
