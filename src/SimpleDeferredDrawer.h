#pragma once
#include "common.h"
#include "IDrawer.h"

#include "RenderTarget.h"
#include "Scene.h"

namespace sunty
{
	class SimpleDeferredDrawer :
		public IDrawer
	{
	public:
		SimpleDeferredDrawer();
		virtual ~SimpleDeferredDrawer();
		virtual void init(const Starter & config) override;
		virtual void update(float delta) override;
		virtual void draw() override;
		virtual void release() override;
		virtual std::shared_ptr<RenderTarget> renderTarget() override;
	private:
		Scene mScene;
		std::shared_ptr<RenderTarget> mGeometryPassRT;
		std::shared_ptr<RenderTarget> mLightingPassRT;
	};

}
