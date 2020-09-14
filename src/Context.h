#pragma once

#include "common.h"
#include "ResourceManager.h"

namespace sunty
{
    class Context
    {
    public:
        static Context & get();
        ResourceManager & resource();
    private:
        Context() = default;
        ResourceManager mResourceManager;
    };
}