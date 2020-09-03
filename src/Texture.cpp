#include "Texture.h"

using namespace sunty;

Texture::Texture()
{

}

Texture::~Texture()
{
    release();
}

bool Texture::setup(const Option &option)
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
    case PL_DEPTH:
        pixelFormat = GL_DEPTH_COMPONENT;
        internalPixelFormat = GL_DEPTH_COMPONENT;
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
        if (option.data != nullptr)
        {
            return false;
        }
        inputDataType = GL_UNSIGNED_BYTE;
        break;
    }
    mTarget = GL_TEXTURE_2D;
    glGenTextures(1, &mTex);
    glBindTexture(mTarget, mTex);
    glTexImage2D(
        mTarget,
        0,
        internalPixelFormat,
        option.width,
        option.height,
        0,
        pixelFormat,
        inputDataType,
        option.data);
    glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(mTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(mTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    mOption = option;
    mOption.data = nullptr;
    return true;
}

void Texture::release()
{
    if (mTex != 0)
    {
        glDeleteTextures(1, &mTex);
        mTex = 0;
    }
}
