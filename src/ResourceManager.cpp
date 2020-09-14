#include "ResourceManager.h"
#include "ImageUtils.h"
#include "SimpleObjLoader.h"
#include "Shader.h"

using namespace sunty;


std::shared_ptr<IResource> ResourceManager::queryTexture(
    const std::filesystem::path & path, 
    const char * pixelFormat)
{
    size_t h0 = std::hash<std::string>{}("Texture");
    size_t h1 = std::hash<std::string>{}(path.lexically_normal().string());
    size_t h2 = std::hash<std::string>{}(pixelFormat);
    size_t h = h0 ^ h1 ^ h2;

    std::shared_ptr<IResource> ptr;
    if (mResourceMap.find(h) != mResourceMap.end())
    {
        ptr = mResourceMap[h].lock();
        if (ptr != nullptr)
        {
            return ptr;
        }
        else
        {
            mResourceMap.erase(h);
        }
    }
    auto texture = std::make_shared<Texture>();
    if (!loadPngTexture(path, pixelFormat, *texture))
    {
        printf("Failed to load png: %s\n", path.string().c_str());
        return ptr;
    }
    ptr = std::static_pointer_cast<IResource>(texture);
    mResourceMap[h] = ptr;
    printf("Texture loaded successfully: %s\n", path.string().c_str());
    return ptr;
}
std::shared_ptr<IResource> ResourceManager::queryCubemap(const std::filesystem::path & path, const char * pixelFormat)
{
    size_t h0 = std::hash<std::string>{}("Cubemap");
    size_t h1 = std::hash<std::string>{}(path.lexically_normal().string());
    size_t h2 = std::hash<std::string>{}(pixelFormat);
    size_t h = h0 ^ h1 ^ h2;

    std::shared_ptr<IResource> ptr;
    if (mResourceMap.find(h) != mResourceMap.end())
    {
        ptr = mResourceMap[h].lock();
        if (ptr != nullptr)
        {
            return ptr;
        }
        else
        {
            mResourceMap.erase(h);
        }
    }
    auto texture = std::make_shared<Cubemap>();
    if (!loadPngCubemap(path, pixelFormat, *texture))
    {
        printf("Failed to load png: %s\n", path.string().c_str());
        return ptr;
    }
    ptr = std::static_pointer_cast<IResource>(texture);
    mResourceMap[h] = ptr;
    printf("Cubemap loaded successfully: %s\n", path.string().c_str());
    return ptr;
}
std::shared_ptr<IResource> ResourceManager::queryMesh(const std::filesystem::path & path, bool hasTangent)
{
    size_t h0 = std::hash<std::string>{}("Mesh");
    size_t h1 = std::hash<std::string>{}(path.lexically_normal().string());
    size_t h2 = std::hash<bool>{}(hasTangent);
    size_t h = h0 ^ h1 ^ h2;

    std::shared_ptr<IResource> ptr;
    if (mResourceMap.find(h) != mResourceMap.end())
    {
        ptr = mResourceMap[h].lock();
        if (ptr != nullptr)
        {
            return ptr;
        }
        else
        {
            mResourceMap.erase(h);
        }
    }
    SimpleObjLoader loader;
    auto mesh = std::make_shared<Mesh>();
    if (!loader.load(path, hasTangent, *mesh))
    {
        printf("Failed to load obj mesh: %s\n", path.string().c_str());
        return ptr;
    }
    ptr = std::static_pointer_cast<IResource>(mesh);
    mResourceMap[h] = ptr;
    printf("Mesh loaded successfully: %s\n", path.string().c_str());
    return ptr;
}
std::shared_ptr<IResource> ResourceManager::queryShader(
    const std::filesystem::path & pathVert, 
    const std::filesystem::path & pathFrag)
{
    size_t h0 = std::hash<std::string>{}("Shader");
    size_t h1 = std::hash<std::string>{}(pathVert.lexically_normal().string());
    size_t h2 = std::hash<std::string>{}(pathFrag.lexically_normal().string());
    size_t h = h0 ^ h1 ^ h2;

    std::shared_ptr<IResource> ptr;
    if (mResourceMap.find(h) != mResourceMap.end())
    {
        ptr = mResourceMap[h].lock();
        if (ptr != nullptr)
        {
            return ptr;
        }
        else
        {
            mResourceMap.erase(h);
        }
    }
    std::string textVert, textFrag;
    if (!loadText(pathVert, textVert))
    {
        printf("Failed to load %s\n", pathVert.string().c_str());
        return ptr;
    }
    if (!loadText(pathFrag, textFrag))
    {
        printf("Failed to load %s\n", pathFrag.string().c_str());
        return ptr;
    }
    auto shader = std::make_shared<Shader>();
    if (!shader->setup(textVert.c_str(), textFrag.c_str()))
    {
        printf("Failed to create shader: %s|%s\n", pathVert.string().c_str(), pathFrag.string().c_str());
        return ptr;
    }
    ptr = std::static_pointer_cast<IResource>(shader);
    mResourceMap[h] = ptr;
    printf("Shader loaded successfully: %s|%s\n", pathVert.string().c_str(), pathFrag.string().c_str());
    return ptr;
}