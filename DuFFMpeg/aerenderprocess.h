#ifndef AERENDEROBJECT_H
#define AERENDEROBJECT_H

#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QStandardPaths>
#include <QVersionNumber>

#include "utils.h"

class AERenderProcess : public QObject
{
    Q_OBJECT
public:
    AERenderProcess(QString path = "", QObject *parent = nullptr);
    ~AERenderProcess();

    /**
     * @brief name The name of this Renderer
     * @return
     */
    QString name() const;
    /**
     * @brief path The path to the binary
     * @return
     */
    QString path() const;
    /**
     * @brief isValid True if the binary has been found and works correctly
     * @return
     */
    bool isValid() const;
    /**
     * @brief dataPath The path to the appdata for After Effects
     * @return
     */
    QString dataPath() const;
    /**
     * @brief version The version of After Effects
     * @return
     */
    QVersionNumber version() const;
    /**
    * @brief aeOutput The output from the binary
    * @return
    */
    QString aeOutput() const;

signals:
    /**
     * @brief newDebug Outputs useful debug informations to be printed in the console
     */
    void newDebug(QString);

private slots:
    void init();
    // Gets and relays the After Effects process errors
    void errorOccurred(QProcess::ProcessError e);
    // Gets and relays the After Effects standard output
    void stdOutput();
    // Gets and relays the After Effects standard error
    void stdError();
    //Sets the DuME Render Templates for After Effects
    bool setDuMETemplates();
    // Restores the original render templates
    void restoreOriginalTemplates();
    // Rendering started
    void started();

    void readyReadAE(QString output);
private:
    // The name of this renderer process
    QString _name;
    // path to the After Effects renderer binary
    QString _path;
    // The version of After Effects
    QVersionNumber _version;
    // The path in appData (to find/replace the templates)
    QString _dataPath;
    // Wether this version works/is found
    bool _isValid;
    // The Process
    QProcess *_aerender;
    // The output
    QString _aeOutput;

    void findDataPath();
};

#endif // AERENDEROBJECT_H
