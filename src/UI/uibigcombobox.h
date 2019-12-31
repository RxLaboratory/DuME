#ifndef UIBIGCOMBOBOX_H
#define UIBIGCOMBOBOX_H

#include <QComboBox>

class UIBigComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit UIBigComboBox(QWidget *parent = nullptr);
    QSize sizeHint() const override        { return minimumSizeHint(); }
    QSize minimumSizeHint() const override { return QSize(50, QComboBox::minimumSizeHint().height()); }

signals:

};

#endif // UIBIGCOMBOBOX_H
