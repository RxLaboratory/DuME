#ifndef DUQFTOOLBARSPACER_H
#define DUQFTOOLBARSPACER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QSpacerItem>

class DuQFToolBarSpacer : public QWidget
{
public:
    explicit DuQFToolBarSpacer(QWidget *parent = nullptr);

private:
    void setupUi();
};


#endif // DUQFTOOLBARSPACER_H
