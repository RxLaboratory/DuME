#include "duapplication.h"

void MessageHandler::messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
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

InitApplication::InitApplication(QString version)
{
    //load settings
    QCoreApplication::setOrganizationName(STR_COMPANYNAME);
    QCoreApplication::setOrganizationDomain(STR_COMPANYDOMAIN);
    QCoreApplication::setApplicationName(STR_PRODUCTNAME);
    QCoreApplication::setApplicationVersion(version);
}

DuApplication::DuApplication(int &argc, char *argv[]) : QApplication(argc, argv)
{
    // handles messages from the app and redirects them to stdout (info) or stderr (debug, warning, critical, fatal)
#ifndef QT_DEBUG
    qInstallMessageHandler(MessageHandler::messageOutput);
#endif

    //set style
    DuUI::updateCSS(":/styles/default", QString(STR_INTERNALNAME).toLower());

    //create splash screen
    QString v = QString(STR_INTERNALNAME);
    QPixmap pixmap(":/images/splash");
    _splashScreen = new DuSplashScreen(pixmap);

    //set app icon
    qApp->setWindowIcon(QIcon(":/icons/app"));
}

DuApplication::DuApplication(int &argc, char *argv[], QString version) : QApplication(argc, argv)
{
    // handles messages from the app and redirects them to stdout (info) or stderr (debug, warning, critical, fatal)
    qInstallMessageHandler(MessageHandler::messageOutput);
    qDebug() << "Initializing application";

    //set style
    DuUI::updateCSS(":/styles/default", QString(STR_INTERNALNAME).toLower());

    //create splash screen
    QString v = QString(STR_INTERNALNAME) + " v";
    QPixmap pixmap(":/images/splash");
    _splashScreen = new DuSplashScreen(pixmap, v + version);

    //set app icon
    qApp->setWindowIcon(QIcon(":/icons/app"));
}

DuSplashScreen *DuApplication::splashScreen() const
{
    return _splashScreen;
}

void DuApplication::showSplashScreen()
{
    qDebug() << "Displaying splash screen";
    _splashScreen->show();
}
