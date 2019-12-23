#include "aerenderer.h"

AERenderer::AERenderer(QString aerender, QObject *parent) : AbstractRenderer(parent)
{
    setBinary( aerender );
    _duration = 0;
}

void AERenderer::readyRead(QString output)
{
    QRegularExpression reProgress("PROGRESS:  \\d:\\d\\d:\\d\\d:\\d\\d \\((\\d+)\\)");
    QRegularExpressionMatch match = reProgress.match(output);
    if (match.hasMatch())
    {
        int currentFrame = match.captured(1).toInt();
        setCurrentFrame( currentFrame );
    }

    //Duration
    QRegularExpression reDuration("PROGRESS:  Duration: (\\d):(\\d\\d):(\\d\\d):(\\d\\d)");
    match = reDuration.match(output);
    if (match.hasMatch())
    {
        int h = match.captured(1).toInt();
        int m = match.captured(2).toInt();
        int s = match.captured(3).toInt();
        int i = match.captured(4).toInt();

        _duration = i/_frameRate + s + m*60 + h*60*60;
        double n = _duration * _frameRate;
        _numFrames = int(n);
    }

    QRegularExpression reFrameRate("PROGRESS:  Frame Rate: (\\d+,\\d\\d)");
    match = reFrameRate.match(output);
    if (match.hasMatch())
    {
        _frameRate = match.captured(1).toDouble();

        // update numframes
        double n = _duration * _frameRate;
        _numFrames = int(n);
    }

    emit progress();
}
