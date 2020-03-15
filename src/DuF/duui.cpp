#include "duui.h"

DuUI::DuUI()
{

}

void DuUI::updateCSS( QString cssFileName, QString appName)
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
    QString css = DuUI::loadCSS(cssFiles);
    qApp->setStyleSheet(css);
}

QString DuUI::loadCSS(QString cssFileName)
{
    return loadCSS(QStringList(cssFileName));
}

QString DuUI::loadCSS(QStringList cssFileNames)
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
    qDebug() << "DuUI: CSS Ready";
    qDebug() << css;
#endif

    return css;
}

void DuUI::setFont(QString family)
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
