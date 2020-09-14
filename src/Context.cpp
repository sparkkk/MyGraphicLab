#include "Context.h"

using namespace sunty;

Context & Context::get()
{
    static Context singleton;
    return singleton;
}

ResourceManager & Context::resource()
{
    return mResourceManager;
}