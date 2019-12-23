#ifndef AERENDERER_H
#define AERENDERER_H

#include "abstractrenderer.h"

#include <QObject>
#include <QRegularExpression>

class AERenderer : public AbstractRenderer
{
public:
    AERenderer(QString aerender, QObject *parent = nullptr);

private:
    // Ae returns the duration and not the numframes
    double _duration;

    // reimplementation from AbstractRenderer to handle ae output
    void readyRead(QString output);
};

#endif // AERENDERER_H
