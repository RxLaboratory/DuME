#ifndef FFPIXFORMAT_H
#define FFPIXFORMAT_H

#include "ffbaseobject.h"


class FFPixFormat : public FFBaseObject
{
    Q_OBJECT

public:
    /**
     * @brief The type of media handled by the codec
     */
    enum Ability { Input = 1 << 0,
                   Output = 1 << 1,
                   Hardware = 1 << 2,
                   Paletted = 1 << 3,
                   Bitstream = 1 << 4
                 };
    Q_DECLARE_FLAGS(Abilities, Ability)

    enum ColorSpace { RGB, YUV, Other };
    Q_ENUM( ColorSpace )

    /**
     * @brief FFPixFormat Constructs a pixFormat instance for FFmpeg
     * @param name The internal name used by FFmpeg
     * @param prettyName The pretty name used for user interaction
     */
    FFPixFormat(QString name, QString prettyName = "", int numComponents = 3, int bitsPerPixel = 24, QObject *parent = nullptr);

    /**
     * @brief FFPixFormat Constructs a pixFormat instance for FFmpeg
     * @param name The internal name used by FFmpeg
     * @param prettyName The pretty name used for user interaction
     * @param abilities The abilities of the pixFmt
     */
    FFPixFormat(QString name, QString prettyName, int numComponents, int bitsPerPixel, Abilities abilities, QObject *parent = nullptr);

    void setAbilities(Abilities &abilities);

    void setInput(bool input);
    void setOutput(bool output);
    void setHardware(bool hardware);
    void setPaletted(bool paletted);
    void setBitstream(bool bitstream);
    void setAlpha(bool hasAlpha);

    bool isInput();
    bool isOutput();

    int bitsPerPixel() const;
    int numComponents() const;
    bool hasAlpha() const;
    ColorSpace colorSpace() const;

private:
    Abilities _abilities;
    int _numComponents;
    int _bitsPerPixel;
    bool _hasAlpha;
    ColorSpace _colorSpace;

};

#endif // FFPIXFORMAT_H
