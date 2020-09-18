#pragma once

#include "common.h"
#include "Scene.h"
#include "RenderTarget.h"

namespace sunty
{
    struct Pipeline
    {
        struct Link
        {
            std::string src;
            std::string dst;
        };
        std::vector<Pass> passes;
        std::vector<std::shared_ptr<RenderTarget>> renderTargets;
        std::vector<Link> links;
        std::shared_ptr<RenderTarget> getRenderTargetByPass(Pass pass);
    };
}