#ifndef AERENDERER_H
#define AERENDERER_H

#include "Renderer/abstractrenderer.h"

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
    // Ae returns the duration and not the numframes
    double _duration;

};

#endif // AERENDERER_H
