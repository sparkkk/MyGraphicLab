#pragma once

namespace sunty
{
    enum Pass
    {
        PASS_NONE        = 0x00,
        PASS_DEFAULT     = 0x01,
        PASS_GEOMETRY    = 0x02,
        PASS_LIGHTING    = 0x04,
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