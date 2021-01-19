#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    versionLabel->setText(qApp->applicationName() + " | version: " + qApp->applicationVersion());
    setWindowTitle("About " + qApp->applicationName());
}
