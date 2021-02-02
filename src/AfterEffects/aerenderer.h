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
    AERenderer(QString aerender, QObject *parent = nullptr);

protected:
    // reimplementation from AbstractRenderer to handle ae output
    void readyRead(QString output);

private:
    // The After Effects renderer info
    AfterEffects *_ae;
    // Ae returns the duration and not the numframes
    double _duration;

    bool launchJob();
    void renderAep(MediaInfo *aep, bool audio = false);

};

#endif // AERENDERER_H
