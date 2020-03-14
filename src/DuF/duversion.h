#ifndef DUVERSION_H
#define DUVERSION_H

#include <QVersionNumber>

class DuVersion : public QVersionNumber
{
public:
    DuVersion(int major, int minor, int micro, QString suffix);
    DuVersion(int major, int minor, int micro);
    DuVersion(int major, int minor);
    DuVersion(int major);
    QString getString();
    QString suffix() const;
    void setSuffix(const QString &suffix);

private:
    QString _suffix;
};

#endif // DUVERSION_H
