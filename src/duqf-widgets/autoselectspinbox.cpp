#include "autoselectspinbox.h"

AutoSelectSpinBox::AutoSelectSpinBox(QWidget *parent): QSpinBox(parent)
{
    this->setLineEdit( new AutoSelectLineEdit(this ));
}
