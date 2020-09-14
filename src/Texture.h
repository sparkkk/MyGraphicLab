#pragma once

#include "common.h"
#include "IResource.h"

namespace sunty
{
    class Texture : public IResource
    {
    public:
        enum PixelLayout
        {
            PL_UNDEFINED,
            PL_RGB,
            PL_RGBA,
            PL_DEPTH,
        };
        enum DataType
        {
            DT_UNDEFINED,
            DT_BYTE,
            DT_F16,
            DT_F32,
        };
        struct Option
        {
            Option()
            {
                memset(this, 0, sizeof(*this));
            }
            PixelLayout pixelLayout = PL_RGB;
            DataType internalDataType = DT_BYTE;
            DataType inputDataType = DT_BYTE;
            union
            {
                int width;
                int widths[6];
            };
            union
            {
                int height;
                int heights[6];
            };
            union
            {
                void * data;
                void * datas[6];
            };
        };
    public:
        Texture();
        virtual ~Texture();
        virtual bool setup(const Option & option);
        virtual void release();
        inline GLuint id() { return mTex; }
        inline GLenum target() { return mTarget; }
        inline Option & options()
        {
            return mOption;
        }
    protected:
        GLenum mTarget = 0;
        GLuint mTex = 0;
        Option mOption;
    };
}


