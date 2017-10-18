#include "mainwindow.h"
#include "frameless.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow();
    FrameLess f(w);
    w->show();
    return a.exec();
}
