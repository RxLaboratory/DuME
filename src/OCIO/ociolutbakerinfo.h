#ifndef OCIOLUTBAKERINFO_H
#define OCIOLUTBAKERINFO_H

#include "Renderer/abstractrendererinfo.h"

#include <QSettings>
#include <QDir>
#include <QtDebug>

class OcioLutBakerInfo : public AbstractRendererInfo
{
    Q_OBJECT
public:
    static OcioLutBakerInfo *instance();

public slots:
    /**
     * @brief setBinaryFileName Sets the path to the ociobakelut binary
     * @param path The path to the binary executable file. If empty string, gets the path from application settings or default paths.
     * @return true if the exe is found
     */
    bool setBinary(QString path = "");

protected:
    static OcioLutBakerInfo *_instance;

private:
    /**
     * @brief Singleton
     */
    OcioLutBakerInfo(QObject *parent = nullptr);

    bool checkBinaryPath(QString path);
};

#endif // OCIOLUTBAKERINFO_H
