#ifndef DUQFDOCKTITLE_H
#define DUQFDOCKTITLE_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QToolButton>
#include <QDockWidget>

class DuQFDockTitle : public QWidget
{
    Q_OBJECT
public:
    explicit DuQFDockTitle(QString title, QWidget *parent = nullptr);

    void setTitle(QString title);
    void setIcon(QString icon);

private slots:
    void closeDockWidget();

private:
    QLabel *_titleLabel;
    QLabel *_iconLabel;

};

#endif // DUQFDOCKTITLE_H
