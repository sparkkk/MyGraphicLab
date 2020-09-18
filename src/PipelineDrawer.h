#pragma once

#include "common.h"
#include "IDrawer.h"
#include "Scene.h"
#include "Pipeline.h"

namespace sunty
{
    class PipelineDrawer : public IDrawer
    {
    public:
        PipelineDrawer() = default;
        virtual ~PipelineDrawer() = default;
		virtual void init(const Starter & config) override;
		virtual void update(float delta) override;
		virtual void draw() override;
		virtual void release() override;
		virtual std::shared_ptr<Texture> texture() override;
    private:
        bool get(const std::string & id, UniformValue & value);
        bool set(const std::string & id, const UniformValue & value);
        Scene mScene;
        Pipeline mPipeline;
        std::shared_ptr<Texture> mOutput;
    };
}