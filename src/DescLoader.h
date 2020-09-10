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
			const std::filesystem::path & path,
			RenderObject & render
		);
		bool loadLight(
			const std::filesystem::path & path,
			Scene::Light & light
		);
		bool loadScene(
			const std::filesystem::path & path,
			Scene & scene
		);
		bool loadStarter(
			const std::filesystem::path & path,
			std::string & scenePath
		);
		std::filesystem::path assembllyPath(const std::filesystem::path & path);
		std::vector<std::filesystem::path> searchPaths;
	};

}
