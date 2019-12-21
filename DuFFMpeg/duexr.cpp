#include "duexr.h"
#include <OpenImageIO/imageio.h>
using namespace OIIO;

#include <QtDebug>

DuEXR::DuEXR(QObject *parent) : QObject(parent)
{
    //test
/*
    auto in = ImageInput::open ("/home/duduf/DEV_SRC/DuFFMpeg/Test Medias/EXR (from Ae)/MYT_0046.003_00000.exr");
    if (! in)
        return;
    const ImageSpec &spec = in->spec();
    //get compression data
#ifdef QT_DEBUG
    qDebug() << QString::fromStdString( spec.decode_compression_metadata().first.str() );
    qDebug() << spec.decode_compression_metadata().second;
#endif
    int xres = spec.width;
    int yres = spec.height;
    int channels = spec.nchannels;
    float *pixels = new float [xres * yres * channels];
    in->read_image (TypeDesc::FLOAT, pixels);
    in->close ();

    const char *filename = "/home/duduf/DEV_SRC/DuFFMpeg/Test Medias/EXR (from Ae)/MYT_0046.003_00000b.exr";

    std::unique_ptr<ImageOutput> out = ImageOutput::create (filename);
    if (! out)
        return;
    ImageSpec outSpec (xres, yres, channels, TypeDesc::FLOAT);
    outSpec.attribute("compression","none");
    out->open (filename, spec);
    out->write_image (TypeDesc::FLOAT, pixels);
    out->close ();
*/

    //test copy image
    // Open the input file
    const char *input = "/home/duduf/DEV_SRC/DuFFMpeg/Test Medias/EXR (from Ae)/MYT_0046.003_00001.exr";
    std::unique_ptr<ImageInput> cin = ImageInput::open (input);

    // Make an output spec, identical to the input except for metadata
    ImageSpec out_spec = cin->spec();

#ifdef QT_DEBUG
    qDebug() << "input compression";
    qDebug() << QString::fromStdString( out_spec.decode_compression_metadata().first.str() );
    qDebug() << out_spec.decode_compression_metadata().second;
#endif

    out_spec.attribute("compression","none");

#ifdef QT_DEBUG
    qDebug() << "output compression";
    qDebug() << QString::fromStdString( out_spec.decode_compression_metadata().first.str() );
    qDebug() << out_spec.decode_compression_metadata().second;
#endif

    // Create the output file and copy the image
    const char *output = "/home/duduf/DEV_SRC/DuFFMpeg/Test Medias/EXR (from Ae)/MYT_0046.003_00001b.exr";
    std::unique_ptr<ImageOutput> cout = ImageOutput::create (output);
    cout->open (output, out_spec);
    cout->copy_image (cin.get());

    // Clean up
    cout->close ();
    cin->close ();

}
