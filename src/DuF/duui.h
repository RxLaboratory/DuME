#ifndef DUUI_H
#define DUUI_H

#include <QStringList>
#include <QFileInfo>
#include <QtDebug>
#include <QApplication>
#include <QFontDatabase>
#include <QRegularExpression>

class DuUI
{
public:
    DuUI();
    static void updateCSS(QString cssFileName, QString appName = "");
    /**
     * @brief loadCSS Loads a CSS File
     * If any cssFileName-values.val file is found, uses these values in the CSS
     * Note: in the values file name, the .val extension can be omitted, or replaced by .txt
     * @param cssFileName The file name (with complete path) of the CSS
     * @return the CSS string
     */
    static QString loadCSS(QString cssFileName = ":/styles/default");
    /**
     * @brief loadCSS Loads multiple CSS Files
     * If any cssFileName[0]-values.val file is found, uses these values in the CSS
     * Note: in the values file name, the .val extension can be omitted, or replaced by .txt
     * @param cssFileNames The file names (with complete path) of the CSS
     * @return the CSS string
     */
    static QString loadCSS(QStringList cssFileNames);
    static void setFont(QString family = "Calibri");
};

#endif // DUUI_H
