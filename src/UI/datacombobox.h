#ifndef DATACOMBOBOX_H
#define DATACOMBOBOX_H

#include <QObject>
#include <QComboBox>

class DataComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit DataComboBox(QWidget *parent = nullptr);
    int setCurrentData(QVariant data);

signals:

};

#endif // DATACOMBOBOX_H
