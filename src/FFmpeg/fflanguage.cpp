#include "fflanguage.h"

FFLanguage::FFLanguage(QString name, QObject *parent) : FFBaseObject(name, parent)
{
    setPrettyName( LanguageUtils::get(name));
}

