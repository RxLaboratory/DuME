#ifndef AFTEREFFECTS_H
#define AFTEREFFECTS_H

#include <QObject>
#include <QDir>
#include <QSettings>

#include "Renderer/abstractrendererinfo.h"

#include "AfterEffects/aftereffectsversion.h"

class AfterEffects : public AbstractRendererInfo
{
    Q_OBJECT
public:
    explicit AfterEffects(QObject *parent = nullptr);

    QList<AfterEffectsVersion *> versions() const;

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

private:

    // === ATTRIBUTES ===

    // The list of After Effects renderers found in the system
    QList<AfterEffectsVersion *> _versions;
    // The application settings
    QSettings settings;

    // === METHODS ===

    // Finds all versions of After Effects installed on the system
    void findAeVersions(QString dir);

};

#endif // AFTEREFFECTS_H
