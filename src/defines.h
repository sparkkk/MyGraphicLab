#pragma once

namespace sunty
{
    enum Pass
    {
        PASS_NONE        = 0x00,
        PASS_DEFAULT     = 0x01,
        PASS_GEOMETRY    = 0x02,
        PASS_LIGHTING    = 0x04,
        PASS_DEPTH_1     = 0x0100,
        PASS_DEPTH_2     = 0x0200,
        PASS_DEPTH_3     = 0x0400,
        PASS_DEPTH_4     = 0x0800,
        PASS_DEPTH_5     = 0x1000,
        PASS_DEPTH_6     = 0x2000,
        PASS_DEPTH_7     = 0x4000,
        PASS_DEPTH_8     = 0x8000,
        PASS_DEPTH_MASK  = 0xFF00,
    };

    enum DrawerType
    {
        DRAWER_TYPE_PIPELINE,
    };

    struct Starter
    {
        int width = 800;
        int height = 600;
        int fps = 40;
        DrawerType drawerType = DRAWER_TYPE_PIPELINE;
        std::string scene;
        std::string pipeline;
    };
}