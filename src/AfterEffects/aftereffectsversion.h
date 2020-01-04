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

class AfterEffectsVersion : public QObject
{
    Q_OBJECT
public:
    AfterEffectsVersion(QString path = "", QObject *parent = nullptr);
    ~AfterEffectsVersion();

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

    //Sets the DuME Render Templates for After Effects
    bool setDuMETemplates();
    // Restores the original render templates
    void restoreOriginalTemplates();

signals:
    void newLog( QString, LogUtils::LogType lt = LogUtils::Information );


private slots:
    void init();

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

    void findDataPath();
};

#endif // AERENDEROBJECT_H
