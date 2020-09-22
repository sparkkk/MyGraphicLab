#pragma once
#include "common.h"
#include "IDrawer.h"
#include "RenderTarget.h"
#include "Scene.h"

namespace sunty
{
    class SimpleForwardDrawer : public IDrawer
    {
    public:
        SimpleForwardDrawer();
        virtual ~SimpleForwardDrawer();
        virtual void init(const Starter & config) override;
        virtual void update(float delta) override;
        virtual void draw() override;
        virtual void release() override;
        virtual std::shared_ptr<Texture> texture() override;
    private:
        Scene mScene;
        std::shared_ptr<RenderTarget> mCanvasRT;
    };

}
