#pragma once

#include "common.h"
#include "RenderTarget.h"

namespace sunty
{
	class IDrawer
	{
	public:
		IDrawer();
		virtual ~IDrawer();
		virtual void init(const Starter & config) = 0;
		virtual void update(float delta) = 0;
		virtual void draw() = 0;
		virtual void release() = 0;
		virtual std::shared_ptr<RenderTarget> renderTarget() = 0;
	};
}

