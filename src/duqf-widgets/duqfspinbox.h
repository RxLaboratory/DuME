#ifndef QSLIDERSPINBOX_H
#define QSLIDERSPINBOX_H

#include <QStackedWidget>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QtDebug>

#include "duqf-widgets/duqfslider.h"

class DuQFSpinBox : public QStackedWidget
{
    Q_OBJECT

public:
    explicit DuQFSpinBox(QWidget *parent = nullptr);
    QString suffix() const;
    void setSuffix(const QString &suffix);

    QString prefix() const;
    void setPrefix(const QString &prefix);

    int minimum() const;
    int maximum() const;
    int value() const;

    bool valueVisible() const;
    void showValue(bool showValue);

    DuQFSlider *slider() const;

    QSpinBox *spinBox() const;

public slots:
    void setValue(int value);
    void setMaximum(int max);
    void setMinimum(int min);

signals:
    void valueChanging(int value);
    void valueChanged(int value);

private slots:
    void spinBox_editingFinished();
    void spinBox_valueChanged(int arg1);
    void slider_valueChanging(int arg1);
    void slider_valueChanged(int arg1);
private:
    void setupUi();
    void connectEvents();
    void updateSliderFormat();
    DuQFSlider *_slider;
    QSpinBox *_spinBox;
    QPoint _originalMousePos;
    QString _suffix;
    QString _prefix;
protected:
    void mouseReleaseEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void focusOutEvent(QFocusEvent* event);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // QSLIDERSPINBOX_H
