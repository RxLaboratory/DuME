#ifndef AUTOSELECTSPINBOX_H
#define AUTOSELECTSPINBOX_H

#include <QSpinBox>

#include "duqf-widgets/autoselectlineedit.h"

class AutoSelectSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    AutoSelectSpinBox(QWidget *parent = nullptr);

};

#endif // AUTOSELECTSPINBOX_H
