#pragma once

#include "Texture.h"
#include "Cubemap.h"

namespace sunty
{
    bool loadPngTexture(const char * path, const char * pixelFormat, Texture & tex);
    bool loadPngCubemap(const char * path, const char * pixelFormat, Cubemap & cubemap);
}