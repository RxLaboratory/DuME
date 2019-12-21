#include "dumagick.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

DuMagick::DuMagick(QObject *parent) : QObject(parent)
{

    //test
    InitializeMagick( nullptr );
    //empty image
    Image my_image;
    //load test image
    qDebug() << "loading file";
    my_image.read("/home/duduf/DEV_SRC/DuFFMpeg/Test Medias/EXR (from Ae)/MYT_0046.003_00000.exr");
    //write as tiff
    qDebug() << "writing file";
    my_image.write("/home/duduf/DEV_SRC/DuFFMpeg/Test Medias/EXR (from Ae)/MYT_0046.003_00000.tiff");
    qDebug() << "file written";

}
