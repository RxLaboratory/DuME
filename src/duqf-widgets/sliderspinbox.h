#ifndef QSLIDERSPINBOX_H
#define QSLIDERSPINBOX_H

#include "ui_sliderspinbox.h"

#include <QtDebug>

#include "duqf-widgets/progressslider.h"

class SliderSpinBox : public QWidget, private Ui::SliderSpinBox
{
    Q_OBJECT

public:
    explicit SliderSpinBox(QWidget *parent = nullptr);
    QString suffix() const;
    void setSuffix(const QString &suffix);

    QString prefix() const;
    void setPrefix(const QString &prefix);

    int minimum() const;
    void setMinimum(int min);

    int maximum() const;
    void setMaximum(int max);

    int value() const;
    void setValue(int value);

    bool valueVisible() const;
    void showValue(bool showValue);

signals:
    void sliderMoved(int value);

private slots:
    void on_spinBox_editingFinished();
    void on_spinBox_valueChanged(int arg1);
    void slider_valueChanged(int arg1);
private:
    void updateSliderFormat();
    ProgressSlider *slider;
    QPoint _originalMousePos;
    QString _suffix;
    QString _prefix;
    bool _showValue;
protected:
    void mouseReleaseEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void focusOutEvent(QFocusEvent* event);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // QSLIDERSPINBOX_H
