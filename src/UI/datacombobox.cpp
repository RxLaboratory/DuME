#include "datacombobox.h"

DataComboBox::DataComboBox(QWidget *parent) : QComboBox(parent)
{

}

int DataComboBox::setCurrentData(QVariant data)
{
    for (int i = 0; i < count(); i++)
    {
        if (itemData(i) == data)
        {
            setCurrentIndex(i);
            return i;
        }
    }

    setCurrentIndex( -1 );
    return -1;
}
