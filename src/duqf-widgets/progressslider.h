#ifndef PROGRESSSLIDER_H
#define PROGRESSSLIDER_H

#include <QProgressBar>
#include <QMouseEvent>

class ProgressSlider : public QProgressBar
{
        Q_OBJECT
public:
    ProgressSlider();
signals:
    void valueChanged(int);
protected:
    void mouseMoveEvent ( QMouseEvent * event );
};
#endif // PROGRESSSLIDER_H
