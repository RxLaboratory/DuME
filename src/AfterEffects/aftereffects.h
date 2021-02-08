#ifndef AFTEREFFECTS_H
#define AFTEREFFECTS_H

#include <QObject>
#include <QDir>
#include <QSettings>
#include <QtDebug>

#include "Renderer/abstractrendererinfo.h"

#include "AfterEffects/aftereffectsversion.h"

class AfterEffects : public AbstractRendererInfo
{
    Q_OBJECT
public:
    /**
     * @brief Gets the unique AfterEffects instance
     * @return
     */
    static AfterEffects *instance();

    QList<AfterEffectsVersion *> versions() const;
    QString currentName() const;
    bool useLatest() const;

    AfterEffectsVersion *currentVersion() const;

public slots:
    /**
     * @brief setCurrentAERenderProcess Sets the process (Ae version) to use, based on the settings of the application
     */
    bool setBinary();
    /**
     * @brief setCurrentAERenderProcess Sets the process (Ae version) to use
     * @param name The name of the process to use.
     */
    bool setBinary(QString name);
    //Sets the DuME Render Templates for After Effects
    bool setDuMETemplates();
    // Restores the original render templates
    void restoreOriginalTemplates();

private:
    /**
     * @brief AfterEffects is a singleton, its constructor is private
     * @param parent
     */
    explicit AfterEffects(QObject *parent = nullptr);

    // === ATTRIBUTES ===

    // The list of After Effects renderers found in the system
    QList<AfterEffectsVersion *> _versions;
    // The application settings
    QSettings settings;
    // The current renderer name
    QString _currentName;
    // Wether to use the latest version
    bool _useLatest;
    // The version currently used
    AfterEffectsVersion *_currentVersion;

    // === METHODS ===

    // Finds all versions of After Effects installed on the system
    void findAeVersions(QString dir);

protected:
    static AfterEffects *_instance;

};

#endif // AFTEREFFECTS_H
