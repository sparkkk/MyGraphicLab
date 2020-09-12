#pragma once

namespace sunty
{
    enum Pass
    {
        PASS_DEFAULT = 0,
        PASS_GEOMETRY,
        PASS_LIGHTING,
    };

    enum DrawerType
    {
        DRAWER_TYPE_FORWARD,
        DRAWER_TYPE_DEFERRED,
    };

    struct Starter
    {
        int width = 800;
        int height = 600;
        int fps = 40;
        DrawerType drawerType = DRAWER_TYPE_FORWARD;
        std::string path;
    };
}