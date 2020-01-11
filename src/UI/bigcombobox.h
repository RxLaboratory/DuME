#ifndef UIBIGCOMBOBOX_H
#define UIBIGCOMBOBOX_H

#include "UI/datacombobox.h"

class BigComboBox : public DataComboBox
{
    Q_OBJECT
public:
    explicit BigComboBox(QWidget *parent = nullptr);
    QSize sizeHint() const override        { return minimumSizeHint(); }
    QSize minimumSizeHint() const override { return QSize(50, QComboBox::minimumSizeHint().height()); }

signals:

};

#endif // UIBIGCOMBOBOX_H
