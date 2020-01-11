#include "datacombobox.h"

DataComboBox::DataComboBox(QWidget *parent) : QComboBox(parent)
{

}

void DataComboBox::setCurrentData(QVariant data)
{
    for (int i = 0; i < count(); i++)
    {
        if (itemData(i) == data)
        {
            setCurrentIndex(i);
            return;
        }
    }

    setCurrentIndex( -1 );
}
