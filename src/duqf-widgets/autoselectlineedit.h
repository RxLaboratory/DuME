#ifndef AUTOSELECTLINEEDIT_H
#define AUTOSELECTLINEEDIT_H

#include <QLineEdit>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QtDebug>

class AutoSelectLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    AutoSelectLineEdit(QWidget *parent = nullptr);

protected:
    void focusInEvent(QFocusEvent *event);
    void mousePressEvent(QMouseEvent *me);
private:
    bool _selectOnMousePress;
};

#endif // AUTOSELECTLINEEDIT_H
