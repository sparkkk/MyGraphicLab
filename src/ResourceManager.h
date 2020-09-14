#pragma once

#include "common.h"
#include "IResource.h"

namespace sunty
{
    class ResourceManager
    {
    public:
        ResourceManager() = default;
        ~ResourceManager() = default;
        std::shared_ptr<IResource> queryTexture(const std::filesystem::path & path, const char * pixelFormat);
        std::shared_ptr<IResource> queryCubemap(const std::filesystem::path & path, const char * pixelFormat);
        std::shared_ptr<IResource> queryMesh(const std::filesystem::path & path, bool hasTangent);
        std::shared_ptr<IResource> queryShader(const std::filesystem::path & pathVert, const std::filesystem::path & pathFrag);
    private:
        std::unordered_map<size_t, std::weak_ptr<IResource>> mResourceMap;
    };
}