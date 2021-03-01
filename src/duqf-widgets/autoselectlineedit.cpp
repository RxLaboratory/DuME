#include "autoselectlineedit.h"

AutoSelectLineEdit::AutoSelectLineEdit(QWidget *parent): QLineEdit(parent)
{
    _selectOnMousePress = false;
}

void AutoSelectLineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    this->selectAll();
    _selectOnMousePress = true;
}

void AutoSelectLineEdit::mousePressEvent(QMouseEvent *me)
{
    QLineEdit::mousePressEvent(me);
    if(_selectOnMousePress) {
        this->selectAll();
        _selectOnMousePress = false;
    }
}
