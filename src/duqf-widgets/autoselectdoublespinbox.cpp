#include "autoselectdoublespinbox.h"

AutoSelectDoubleSpinBox::AutoSelectDoubleSpinBox(QWidget *parent): QDoubleSpinBox(parent)
{
    this->setLineEdit( new AutoSelectLineEdit(this ));
}
