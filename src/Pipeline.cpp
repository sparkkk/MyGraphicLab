#include "Pipeline.h"

using namespace sunty;

std::shared_ptr<RenderTarget> Pipeline::getRenderTargetByPass(Pass pass)
{
    for (int i = 0; i < passes.size(); ++i)
    {
        if (passes[i] == pass)
        {
            return renderTargets[i];
        }
    }
    return std::shared_ptr<RenderTarget>();
}