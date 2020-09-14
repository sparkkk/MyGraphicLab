#include "ImageUtils.h"

#include "lodepng.h"

namespace sunty
{

bool loadText(const std::filesystem::path & path, std::string & text)
{
    std::ifstream stream;
    stream.open(path);
    if (!stream.is_open())
    {
        return false;
    }
    auto size = std::streamoff(stream.seekg(0, std::ios::end).tellg());
    text.resize(size + 1);
    stream.seekg(0, std::ios::beg);
    stream.read(const_cast<char*>(text.data()), size);
    text[size] = 0;
    stream.close();
    return true;
}

static std::vector<uint8_t> flipImage(const std::vector<uint8_t> & image, int stride, int height)
{
    std::vector<uint8_t> flip;
    flip.resize(image.size());
    const uint8_t * p = image.data() + (height - 1) * stride;
    uint8_t * q = flip.data();
    for (int i = 0; i < height; ++i)
    {
        memcpy(q, p, stride);
        p -= stride;
        q += stride;
    }
    return flip;
}

bool loadPngTexture(const std::filesystem::path & path, const char * pixelFormat, Texture & tex)
{
    LodePNGColorType colorType = LodePNGColorType::LCT_RGB;
    Texture::PixelLayout pixelLayout = Texture::PL_RGB;
    Texture::DataType pixelType = Texture::DT_BYTE;
    size_t pixelSize = 0;

    std::string strPixelFormat = pixelFormat;
    if (strPixelFormat == "RGB")
    {
        colorType = LodePNGColorType::LCT_RGB;
        pixelLayout = Texture::PL_RGB;
        pixelSize = 3;
    }
    else if (strPixelFormat == "RGBA")
    {
        colorType = LodePNGColorType::LCT_RGBA;
        pixelLayout = Texture::PL_RGBA;
        pixelSize = 4;
    }

    std::vector<uint8_t> image;
    uint32_t imageW, imageH;
    //actually load from left-top
    auto err = lodepng::decode(
        image,
        imageW,
        imageH,
        path.string(),
        colorType,
        8U
    );
    if (err != 0)
    {
        return false;
    }
    //printf("png first pixel: %d %d %d\n", image[0], image[1], image[2]);
    //printf("png last pixel: %d %d %d\n", image[image.size()-3], image[image.size()-2], image[image.size()-1]);
    image = std::move(flipImage(image, pixelSize * imageW, imageH));
    Texture::Option option;
    option.width = imageW;
    option.height = imageH;
    option.pixelLayout = pixelLayout;
    option.internalDataType = pixelType;
    option.inputDataType = Texture::DT_BYTE;
    option.data = image.data();
    tex.setup(option);
    return true;
}

bool loadPngCubemap(const std::filesystem::path & path, const char * pixelFormat, Cubemap & cubemap)
{
    const char * suffixes[] = {
        "-pos-x",
        "-neg-x",
        "-pos-y",
        "-neg-y",
        "-pos-z",
        "-neg-z",
    };

    LodePNGColorType colorType = LodePNGColorType::LCT_RGB;
    Texture::PixelLayout pixelLayout = Texture::PL_RGB;
    Texture::DataType pixelType = Texture::DT_BYTE;
    size_t pixelSize = 0;

    std::string strPixelFormat = pixelFormat;
    if (strPixelFormat == "RGB")
    {
        colorType = LodePNGColorType::LCT_RGB;
        pixelLayout = Texture::PL_RGB;
        pixelSize = 3;
    }
    else if (strPixelFormat == "RGBA")
    {
        colorType = LodePNGColorType::LCT_RGBA;
        pixelLayout = Texture::PL_RGBA;
        pixelSize = 4;
    }

    Texture::Option option;
    option.pixelLayout = pixelLayout;
    option.internalDataType = pixelType;
    option.inputDataType = Texture::DT_BYTE;

    std::vector<uint8_t> faceImages[6];
    for (int i = 0; i < 6; ++i)
    {
        std::string pathStr = path.string();
        std::filesystem::path facePath = pathStr.replace(pathStr.find("[f]"), 3, suffixes[i]);

        std::vector<uint8_t> & image = faceImages[i];
        uint32_t imageW, imageH;
        //actually load from left-top
        auto err = lodepng::decode(
            image,
            imageW,
            imageH,
            facePath.string(),
            colorType,
            8U
        );
        if (err != 0)
        {
            printf("cubemap face decode failed: %s\n", facePath.string().c_str());
            return false;
        }
        image = std::move(flipImage(image, pixelSize * imageW, imageH));
        option.widths[i] = imageW;
        option.heights[i] = imageH;
        option.datas[i] = image.data();
    }
    if (!cubemap.setup(option))
    {
        return false;
    }
    return true;
}

}