#ifndef AERENDERER_H
#define AERENDERER_H

#include "Renderer/abstractrenderer.h"
#include "Renderer/cachemanager.h"
#include "AfterEffects/aftereffects.h"

#include <QObject>
#include <QRegularExpression>

class AERenderer : public AbstractRenderer
{
public:
    /**
     * @brief Gets the unique AERenderer instance
     * @return
     */
    static AERenderer *instance();

protected:
    // reimplementation from AbstractRenderer to handle ae output
    void readyRead(QString output);

private:
    /**
     * @brief AERenderer is a singleton, the constructor is private
     * @param parent
     */
    AERenderer(QObject *parent = nullptr);
    /**
     * @brief We need to store the duration in seconds as After Effects uses that instead of frames
     */
    double _duration;
    /**
     * @brief Launches the current job if there is an AEP to render in it, by calling renderAep()
     * @return True if it has been launched, false if there is no AEP to render in the current job
     */
    bool launchJob();
    /**
     * @brief Parses the MediaInfo aep to generate aerender CLI arguments and launches the process.
     * @param aep
     * @param audio
     */
    void renderAep(MediaInfo *aep, bool audio = false);
protected:
    /**
     * @brief The unique AEREnderer instance
     */
    static AERenderer *_instance;
};

#endif // AERENDERER_H
