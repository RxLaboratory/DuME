#ifndef RAINBOXUI_H
#define RAINBOXUI_H

#include <QApplication>
#include <QIcon>

#include "dusplashscreen.h"
#include "duui.h"
#include "version.h"

class MessageHandler
{
public:
    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

class InitApplication
{
public:
    InitApplication(QString version);
};

class DuApplication : public QApplication
{
public:
    DuApplication(int &argc, char *argv[]);
    DuApplication(int &argc, char *argv[], QString version);
    DuSplashScreen *splashScreen() const;
    void showSplashScreen();

private:
    DuSplashScreen *_splashScreen;
};

#endif // RAINBOXUI_H
