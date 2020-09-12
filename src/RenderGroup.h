#pragma once

#include "common.h"
#include "IRenderObject.h"

namespace sunty
{
    class RenderGroup : public IRenderObject
    {
    public:
        void setup(const std::vector<std::shared_ptr<IRenderObject>> & renders);
		virtual void setParam(const char * name, const UniformValue & value) override;
		virtual void draw() override;
    private:
        std::vector<std::shared_ptr<IRenderObject>> mRenders;
    };
}