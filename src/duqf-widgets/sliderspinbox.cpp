#include "sliderspinbox.h"

SliderSpinBox::SliderSpinBox(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _showValue = true;
    _prefix = "";
    _suffix = "";

    slider = new ProgressSlider();
    sliderLayout->addWidget(slider);

    spinBox->installEventFilter(this);

    updateSliderFormat();

    connect(slider,SIGNAL(valueChanged(int)), this, SLOT(slider_valueChanged(int)));

    setValue(0);

    stackedWidget->setCurrentIndex(1);
}

void SliderSpinBox::on_spinBox_editingFinished()
{
    stackedWidget->setCurrentIndex(1);
}

void SliderSpinBox::on_spinBox_valueChanged(int arg1)
{
    slider->setValue(arg1);
    emit sliderMoved(arg1);
}

void SliderSpinBox::slider_valueChanged(int arg1)
{
    spinBox->setValue(arg1);
}

void SliderSpinBox::updateSliderFormat()
{
    QString format;
    if (_showValue) format = _prefix + "%v" + _suffix;
    else format = _prefix;
    slider->setFormat(format);
}

bool SliderSpinBox::valueVisible() const
{
    return _showValue;
}

void SliderSpinBox::showValue(bool showValue)
{
    _showValue = showValue;
    updateSliderFormat();
}

int SliderSpinBox::value() const
{
    return spinBox->value();
}

void SliderSpinBox::setValue(int value)
{
    spinBox->setValue(value);
    slider->setValue(value);
}

int SliderSpinBox::maximum() const
{
    return spinBox->maximum();
}

void SliderSpinBox::setMaximum(int max)
{
    spinBox->setMaximum(max);
    slider->setMaximum(max);
}

int SliderSpinBox::minimum() const
{
    return spinBox->minimum();
}

void SliderSpinBox::setMinimum(int min)
{
    spinBox->setMinimum(min);
    slider->setMinimum(min);
}

QString SliderSpinBox::prefix() const
{
    return _prefix;
}

void SliderSpinBox::setPrefix(const QString &prefix)
{
    _prefix = prefix;
    updateSliderFormat();
}

QString SliderSpinBox::suffix() const
{
    return _suffix;
}

void SliderSpinBox::setSuffix(const QString &suffix)
{
    _suffix = suffix;
    updateSliderFormat();
}

void SliderSpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (stackedWidget->currentIndex() == 0)
    {
        event->ignore();
        return;
    }
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    if (_originalMousePos != event->pos())
    {
        event->ignore();
        return;
    }
    stackedWidget->setCurrentIndex(0);
    spinBox->selectAll();
    spinBox->setFocus();
    event->accept();
}

void SliderSpinBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _originalMousePos = event->pos();
    }

    if (event->y() < 0 || event->y() > height() || event->x() < 0 || event->x() > width())
    {
        stackedWidget->setCurrentIndex(1);
        event->accept();
        return;
    }

    event->ignore();
}

void SliderSpinBox::focusOutEvent(QFocusEvent *event)
{
    stackedWidget->setCurrentIndex(1);
    event->accept();
}

bool SliderSpinBox::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::FocusOut)
  {
      stackedWidget->setCurrentIndex(1);
      return true;
  }
  else
  {
      // standard event processing
      return QObject::eventFilter(obj, event);
  }
}
