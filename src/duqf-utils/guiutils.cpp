#include "guiutils.h"

QMainWindow *GuiUtils::appMainWindow()
{
    foreach(QWidget *w, qApp->topLevelWidgets())
    {
        if (w->inherits("MainWindow"))
        {
            QMainWindow *mw = qobject_cast<QMainWindow*>(w);
            if (mw) return mw;
        }
    }
    return nullptr;
}
