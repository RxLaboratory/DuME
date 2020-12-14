#ifndef LANGUAGEUTILS_H
#define LANGUAGEUTILS_H

#include <QObject>

class LanguageList : QObject
{
    Q_OBJECT
public:
    LanguageList( QObject *parent = nullptr );

    enum LanguageIDType { ISO639_2, ISO639_1, ISO639_3, ENGLISH_NAME, NATIVE_NAME };
    Q_ENUM(LanguageIDType)

    QString get(QString id, LanguageIDType from = ISO639_2, LanguageIDType to = NATIVE_NAME) const;

private:
    QList<QStringList> _languages;
};

namespace LanguageUtils
{
    QString get(QString id, LanguageList::LanguageIDType from = LanguageList::ISO639_2, LanguageList::LanguageIDType to = LanguageList::NATIVE_NAME);
    static const LanguageList *languageList = new LanguageList();
};



#endif // LANGUAGEUTILS_H
