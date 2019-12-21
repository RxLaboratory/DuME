#include "duexr.h"
#include <OpenImageIO/imageio.h>
using namespace OIIO;

#include <QtDebug>

DuEXR::DuEXR(QObject *parent) : QObject(parent)
{
    qDebug() << "load";

    auto in = ImageInput::open ("/home/duduf/DEV_SRC/DuFFMpeg/Test Medias/EXR (from Ae)/MYT_0046.003_00000.exr");
    if (! in)
        return;
    const ImageSpec &spec = in->spec();
    int xres = spec.width;
    int yres = spec.height;
    int channels = spec.nchannels;
    std::vector<unsigned char> pixels (xres*yres*channels);
    in->read_image (TypeDesc::UINT8, &pixels[0]);
    in->close ();

    qDebug() << "write";

    const char *filename = "/home/duduf/DEV_SRC/DuFFMpeg/Test Medias/EXR (from Ae)/MYT_0046.003_00000.jpg";

    std::unique_ptr<ImageOutput> out = ImageOutput::create (filename);
    if (! out)
        return;
    ImageSpec outSpec (xres, yres, channels, TypeDesc::UINT8);
    out->open (filename, spec);
    out->write_image (TypeDesc::UINT8, &pixels[0]);
    out->close ();

}
