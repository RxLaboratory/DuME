#ifndef DUQFGRID_H
#define DUQFGRID_H

#include <QPoint>
#include <QObject>

class DuQFGrid: public QObject
{
    Q_OBJECT
public:
    DuQFGrid(QObject *parent = nullptr);
    void setSize(int gridSize);
    int size() const;
    QPointF snapToGrid(QPointF in) const;
    bool snapEnabled() const;
    int gridSize() const;

public slots:
    void setSnapEnabled(bool snapEnabled);
    void setGridSize(int size);

signals:
    void gridSizeChanged();

private:
    bool _snapEnabled = false;
    int _size = 20;
};

#endif // DUQFGRID_H
