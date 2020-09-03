#pragma once

#include "common.h"
#include "Scene.h"

namespace sunty
{
	class DescLoader
	{
	public:
		DescLoader();
		~DescLoader();

		bool loadRenderObject(
			const char * path,
			RenderObject & render
		);
		bool loadLight(
			const char * path,
			Scene::Light & light
		);
		bool loadScene(
			const char * path,
			Scene & scene
		);
	};

}
