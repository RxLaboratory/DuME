#include "duqf-app/app-style.h"

FrameLessWindow::FrameLessWindow(QMainWindow *target) :
    _target(target),
    _cursorchanged(false),
    _leftButtonPressed(false),
    _borderWidth(10),
    _dragPos(QPoint())
{
    _target->setMouseTracking(true);
    _target->setWindowFlags(Qt::FramelessWindowHint);
    _target->setAttribute(Qt::WA_Hover);
    _target->installEventFilter(this);
    _rubberband = new QRubberBand(QRubberBand::Rectangle);
}

bool FrameLessWindow::eventFilter(QObject *o, QEvent*e) {
    if (e->type() == QEvent::MouseMove)
    {
        mouseMove(static_cast<QMouseEvent*>(e));
        return true;
    }
    else if (e->type() == QEvent::HoverMove)
    {
        mouseHover(static_cast<QHoverEvent*>(e));
        return true;
    }
    else if (e->type() == QEvent::Leave)
    {
        mouseLeave();
        return true;
    }
    else if (e->type() == QEvent::MouseButtonPress)
    {
        mousePress(static_cast<QMouseEvent*>(e));
        return true;
    }
    else if (e->type() == QEvent::MouseButtonRelease)
    {
        mouseRealese(static_cast<QMouseEvent*>(e));
        return true;
    }
    else {
        return _target->eventFilter(o, e);
    }
}

void FrameLessWindow::mouseHover(QHoverEvent *e) {
    updateCursorShape(_target->mapToGlobal(e->pos()));
}

void FrameLessWindow::mouseLeave() {
    if (!_leftButtonPressed) {
        _target->unsetCursor();
    }
}

void FrameLessWindow::mousePress(QMouseEvent *e) {
    if (e->button() & Qt::LeftButton) {
        _leftButtonPressed = true;
        calculateCursorPosition(e->globalPos(), _target->frameGeometry(), _mousePress);
        if (!_mousePress.testFlag(Edge::None)) {
            _rubberband->setGeometry(_target->frameGeometry());
        }
        if (_target->rect().marginsRemoved(QMargins(borderWidth(), borderWidth(), borderWidth(), borderWidth())).contains(e->pos())) {
            _dragStart = true;
            _dragPos = e->pos();
        }
    }
}

void FrameLessWindow::mouseRealese(QMouseEvent *e) {
    if (e->button() & Qt::LeftButton) {
        _leftButtonPressed = false;
        _dragStart = false;
    }
}

void FrameLessWindow::mouseMove(QMouseEvent *e) {
    if (_leftButtonPressed) {
        if (_dragStart) {
            _target->setCursor(Qt::SizeAllCursor);
            _target->move(e->globalPos() - _dragPos);
        }

        if (!_mousePress.testFlag(Edge::None)) {
            int left = _rubberband->frameGeometry().left();
            int top = _rubberband->frameGeometry().top();
            int right = _rubberband->frameGeometry().right();
            int bottom = _rubberband->frameGeometry().bottom();
            switch (_mousePress) {
            case Edge::Top:
                top = e->globalPos().y();
                break;
            case Edge::Bottom:
                bottom = e->globalPos().y();
                break;
            case Edge::Left:
                left = e->globalPos().x();
                break;
            case Edge::Right:
                right = e->globalPos().x();
                break;
            case Edge::TopLeft:
                top = e->globalPos().y();
                left = e->globalPos().x();
                break;
            case Edge::TopRight:
                right = e->globalPos().x();
                top = e->globalPos().y();
                break;
            case Edge::BottomLeft:
                bottom = e->globalPos().y();
                left = e->globalPos().x();
                break;
            case Edge::BottomRight:
                bottom = e->globalPos().y();
                right = e->globalPos().x();
                break;
            }
            QRect newRect(QPoint(left, top), QPoint(right, bottom));
            if (newRect.width() < _target->minimumWidth()) {
                left = _target->frameGeometry().x();
            }
            else if (newRect.height() < _target->minimumHeight()) {
                top = _target->frameGeometry().y();
            }
            _target->setGeometry(QRect(QPoint(left, top), QPoint(right, bottom)));
            _rubberband->setGeometry(QRect(QPoint(left, top), QPoint(right, bottom)));
        }
    }
    else {
        updateCursorShape(e->globalPos());
    }
}

void FrameLessWindow::updateCursorShape(const QPoint &pos) {
    if (_target->isFullScreen() || _target->isMaximized()) {
        if (_cursorchanged) {
            _target->unsetCursor();
        }
        return;
    }
    if (!_leftButtonPressed) {
        calculateCursorPosition(pos, _target->frameGeometry(), _mouseMove);
        _cursorchanged = true;
        if (_mouseMove.testFlag(Edge::Top) || _mouseMove.testFlag(Edge::Bottom)) {
            _target->setCursor(Qt::SizeVerCursor);
        }
        else if (_mouseMove.testFlag(Edge::Left) || _mouseMove.testFlag(Edge::Right)) {
            _target->setCursor(Qt::SizeHorCursor);
        }
        else if (_mouseMove.testFlag(Edge::TopLeft) || _mouseMove.testFlag(Edge::BottomRight)) {
            _target->setCursor(Qt::SizeFDiagCursor);
        }
        else if (_mouseMove.testFlag(Edge::TopRight) || _mouseMove.testFlag(Edge::BottomLeft)) {
            _target->setCursor(Qt::SizeBDiagCursor);
        }
        else if (_cursorchanged) {
            _target->unsetCursor();
            _cursorchanged = false;
        }
    }
}

void FrameLessWindow::calculateCursorPosition(const QPoint &pos, const QRect &framerect, Edges &_edge) {
    bool onLeft = pos.x() >= framerect.x() - _borderWidth && pos.x() <= framerect.x() + _borderWidth &&
        pos.y() <= framerect.y() + framerect.height() - _borderWidth && pos.y() >= framerect.y() + _borderWidth;

    bool onRight = pos.x() >= framerect.x() + framerect.width() - _borderWidth && pos.x() <= framerect.x() + framerect.width() &&
        pos.y() >= framerect.y() + _borderWidth && pos.y() <= framerect.y() + framerect.height() - _borderWidth;

    bool onBottom = pos.x() >= framerect.x() + _borderWidth && pos.x() <= framerect.x() + framerect.width() - _borderWidth  &&
        pos.y() >= framerect.y() + framerect.height() - _borderWidth && pos.y() <= framerect.y() + framerect.height();

    bool onTop = pos.x() >= framerect.x() + _borderWidth && pos.x() <= framerect.x() + framerect.width() - _borderWidth &&
        pos.y() >= framerect.y() && pos.y() <= framerect.y() + _borderWidth;

    bool  onBottomLeft = pos.x() <= framerect.x() + _borderWidth && pos.x() >= framerect.x() &&
        pos.y() <= framerect.y() + framerect.height() && pos.y() >= framerect.y() + framerect.height() - _borderWidth;

    bool onBottomRight = pos.x() >= framerect.x() + framerect.width() - _borderWidth && pos.x() <= framerect.x() + framerect.width() &&
        pos.y() >= framerect.y() + framerect.height() - _borderWidth && pos.y() <= framerect.y() + framerect.height();

    bool onTopRight = pos.x() >= framerect.x() + framerect.width() - _borderWidth && pos.x() <= framerect.x() + framerect.width() &&
        pos.y() >= framerect.y() && pos.y() <= framerect.y() + _borderWidth;

    bool onTopLeft = pos.x() >= framerect.x() && pos.x() <= framerect.x() + _borderWidth &&
        pos.y() >= framerect.y() && pos.y() <= framerect.y() + _borderWidth;

    if (onLeft) {
        _edge = Left;
    }
    else if (onRight) {
        _edge = Right;
    }
    else if (onBottom) {
        _edge = Bottom;
    }
    else if (onTop) {
        _edge = Top;
    }
    else if (onBottomLeft) {
        _edge = BottomLeft;
    }
    else if (onBottomRight) {
        _edge = BottomRight;
    }
    else if (onTopRight) {
        _edge = TopRight;
    }
    else if (onTopLeft) {
        _edge = TopLeft;
    }
    else {
        _edge = None;
    }
}
