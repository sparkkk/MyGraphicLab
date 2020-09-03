#pragma once

#include "common.h"
#include "RenderObject.h"
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
		void update(float delta);
		void draw();
		std::vector<Camera> cameras;
		std::vector<RenderObject> renders;
		std::vector<std::vector<Transform>> transforms;
		std::unordered_map<std::string, float> valueMap;
		std::vector<glm::mat4> matrices;
		std::vector<Light> lights;
		int currentCameraIndex = 0;
		std::vector<bool> follows;
	};
}