#pragma once

#include "common.h"
#include "Scene.h"
#include "RenderObject.h"
#include "Pipeline.h"

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
            const Starter & config,
            Pass passMask,
            Scene & scene
        );
        bool loadPipeline(
            const std::filesystem::path & path,
            const Starter & config,
            Pipeline & pipeline
        );
        bool loadStarter(
            const std::filesystem::path & path,
            Starter & starter
        );
        std::filesystem::path assembllyPath(const std::filesystem::path & path);
        std::vector<std::filesystem::path> searchPaths;
        Starter config;
    };

}
