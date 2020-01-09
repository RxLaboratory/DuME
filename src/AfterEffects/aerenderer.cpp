#include "aerenderer.h"
#include <QtDebug>

AERenderer::AERenderer(QString aerender, QObject *parent) : AbstractRenderer(parent)
{
    _duration = 0;
    setBinary( aerender ); 
}

void AERenderer::readyRead(QString output)
{
    emit console(output);

    QRegularExpression reProgress("PROGRESS:\\s*[\\d:]+\\s*\\((\\d+)\\)");
    QRegularExpressionMatch match = reProgress.match(output);
    if (match.hasMatch())
    {
        int currentFrame = match.captured(1).toInt();
        setCurrentFrame( currentFrame );
    }

    //Duration
    QRegularExpression reDuration("PROGRESS:\\s*Duration:\\s*(?:(?:(\\d):(\\d\\d):(\\d\\d):(\\d\\d))|(\\d+))");
    match = reDuration.match(output);
    if (match.hasMatch())
    {
        if (match.captured(5) == "")
        {
            int h = match.captured(1).toInt();
            int m = match.captured(2).toInt();
            int s = match.captured(3).toInt();
            int i = match.captured(4).toInt();

            if (_frameRate != 0)
            {
                _duration = i/_frameRate + s + m*60 + h*60*60;
                double n = _duration * _frameRate;
                _numFrames = int(n);
            }
            else
            {
                _duration = i/24 + s + m*60 + h*60*60;
                double n = _duration * 24;
                _numFrames = int(n);
            }

        }
        else
        {
            _numFrames = match.captured(5).toInt();
            if (_frameRate != 0) _duration = _numFrames / _frameRate;
            else _duration = _numFrames / 24;
        }
    }

    // Frame Rate
    QRegularExpression reFrameRate("PROGRESS:  Frame Rate: (\\d+,\\d\\d)");
    match = reFrameRate.match(output);
    if (match.hasMatch())
    {
        _frameRate = match.captured(1).toDouble();
    }

    emit progress();
}
