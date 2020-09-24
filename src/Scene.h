#pragma once

#include "common.h"
#include "IRenderObject.h"
#include "Camera.h"

namespace sunty
{
    struct Scene
    {
        struct Light
        {
            enum Type
            {
                TYPE_POINT,
            };
            Type type;
            std::unordered_map<std::string, UniformValue> uniforms;
        };
        struct Transform
        {
            enum Type
            {
                TYPE_SCALE,
                TYPE_ROTATE,
                TYPE_TRANSLATE
            };
            Type type;
            std::vector<std::vector<std::string>> exprs;
        };
        Scene();
        ~Scene();
        void setup();
        void updateCommon(float delta);
        void updatePass(float delta);
        void draw();
        void setCurrentPass(Pass pass);
        Camera * getCameraByPass(Pass pass);
        std::vector<Camera> cameras;
        std::vector<std::shared_ptr<IRenderObject>> renders;
        std::vector<std::vector<Transform>> transforms;
        std::unordered_map<std::string, float> valueMap;
        std::vector<glm::mat4> matrices;
        std::vector<Light> lights;
        std::vector<bool> follows;
    private:
        Pass pass = PASS_DEFAULT;
    };
}