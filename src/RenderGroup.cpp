#include "RenderGroup.h"

using namespace sunty;

void RenderGroup::setup(const std::vector<std::shared_ptr<IRenderObject>> & renders)
{
    mRenders = renders;
}

void RenderGroup::setParam(const char * name, const UniformValue & value)
{
    for (auto r : mRenders)
    {
        if (r->getPassMask() & passCurr)
        {
            r->setParam(name, value);
        }
    }
}

void RenderGroup::draw()
{
    for (auto r : mRenders)
    {
        if (r->getPassMask() & passCurr)
        {
            r->draw();
        }
    }
}