#include "duqfgrid.h"

DuQFGrid::DuQFGrid(QObject *parent): QObject(parent)
{

}

void DuQFGrid::setSize(int size)
{
    _size = size;
}

int DuQFGrid::size() const
{
    return _size;
}

QPointF DuQFGrid::snapToGrid(QPointF in) const
{
    if (!_snapEnabled) return in;
    if (!_size) return in;

    return {
        static_cast<double>(static_cast<int>(in.x() / _size) * _size),
        static_cast<double>(static_cast<int>(in.y() / _size) * _size)
    };
}

bool DuQFGrid::snapEnabled() const
{
    return _snapEnabled;
}

int DuQFGrid::gridSize() const
{
    return _size;
}

void DuQFGrid::setSnapEnabled(bool snapEnabled)
{
    _snapEnabled = snapEnabled;
}

void DuQFGrid::setGridSize(int size)
{
    _size = size;
    emit gridSizeChanged();
}
