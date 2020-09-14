#pragma once

#include "common.h"
#include "Texture.h"
#include "Cubemap.h"

namespace sunty
{
    bool loadText(const std::filesystem::path & path, std::string & text);
    bool loadPngTexture(const std::filesystem::path & path, const char * pixelFormat, Texture & tex);
    bool loadPngCubemap(const std::filesystem::path & path, const char * pixelFormat, Cubemap & cubemap);
}