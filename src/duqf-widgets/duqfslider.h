#ifndef DUQFSLIDER_H
#define DUQFSLIDER_H

#include <QProgressBar>
#include <QMouseEvent>

class DuQFSlider : public QProgressBar
{
        Q_OBJECT
public:
    DuQFSlider(QWidget *parent = nullptr);

    QString prefix() const;
    void setPrefix(const QString &prefix);

    QString suffix() const;
    void setSuffix(const QString &suffix);

    virtual QString text() const override;

    bool valueVisible() const;
    void showValue(bool showValue);

    void setEditable(bool editable);

signals:
    void valueChanging(int);
    void editingFinished(int);
private:
    QString _prefix;
    QString _suffix;
    bool _showValue;
    bool _editable = true;
protected:
    void mouseMoveEvent ( QMouseEvent * event ) override;
    void mouseReleaseEvent( QMouseEvent * event ) override;
};
#endif // DUQFSLIDER_H
