#ifndef AFTEREFFECTS_H
#define AFTEREFFECTS_H

#include <QObject>
#include <QDir>
#include <QSettings>

#include "AfterEffects/aftereffectsversion.h"

class AfterEffects : public QObject
{
    Q_OBJECT
public:
    explicit AfterEffects(QObject *parent = nullptr);

    QList<AfterEffectsVersion *> versions() const;

    /**
     * @brief setCurrentAERenderProcess Sets the process (Ae version) to use, based on the settings of the application
     */
    void setCurrentAERenderProcess();
    /**
     * @brief setCurrentAERenderProcess Sets the process (Ae version) to use
     * @param name The name of the process to use.
     */
    void setCurrentAERenderProcess(QString name);

    QString binary() const;

signals:
    void newLog(QString);
    void currentAeVersionChanged(QString aerender);

private:

    // === ATTRIBUTES ===

    // The list of After Effects renderers found in the system
    QList<AfterEffectsVersion *> _versions;
    // The application settings
    QSettings settings;
    // The binary
    QString _binary;

    // === METHODS ===

    // Finds all versions of After Effects installed on the system
    void findAeVersions(QString dir);

};

#endif // AFTEREFFECTS_H
