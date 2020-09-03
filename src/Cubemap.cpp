#include "Cubemap.h"

using namespace sunty;

Cubemap::Cubemap()
{
}

bool Cubemap::setup(const Option &option)
{
    GLenum internalPixelFormat = 0;
    GLenum pixelFormat = 0;
    GLenum inputDataType = 0;
    switch (option.pixelLayout)
    {
    case PL_RGB:
        pixelFormat = GL_RGB;
        switch (option.internalDataType)
        {
        case DT_BYTE:
            internalPixelFormat = GL_RGB;
            break;
        case DT_F16:
            internalPixelFormat = GL_RGB16F;
            break;
        default:
            return false;
        }
        break;
    case PL_RGBA:
        pixelFormat = GL_RGBA;
        switch (option.internalDataType)
        {
        case DT_BYTE:
            internalPixelFormat = GL_RGBA;
            break;
        case DT_F16:
            internalPixelFormat = GL_RGBA16F;
            break;
        default:
            return false;
        }
        break;
    default:
        return false;
    }
    switch (option.inputDataType)
    {
    case DT_BYTE:
        inputDataType = GL_UNSIGNED_BYTE;
        break;
    case DT_F32:
        inputDataType = GL_FLOAT;
        break;
    default:
        for (int i = 0; i < 6; ++i)
        {
            if (option.datas[i] != nullptr)
            {
                return false;
            }
        }
        inputDataType = GL_UNSIGNED_BYTE;
        break;
    }
    if (mTex == 0)
    {
        glGenTextures(1, &mTex);
    }
    mTarget = GL_TEXTURE_CUBE_MAP;
    glBindTexture(mTarget, mTex);

    GLint targets[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    };
    for (int i = 0; i < 6; ++i)
    {
        glTexImage2D(
            targets[i],
            0,
            internalPixelFormat,
            option.widths[i],
            option.heights[i],
            0,
            pixelFormat,
            inputDataType,
            option.datas[i]
        );
    }
    glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(mTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(mTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(mTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    mOption = option;
    memset(mOption.datas, 0, sizeof(mOption.datas));
    return true;
}