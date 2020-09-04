#pragma once

#include "common.h"
#include "RenderTarget.h"

namespace sunty
{
	class IDrawer
	{
	public:
		struct Config
		{
			int width;
			int height;
		};
		struct ObjectParam
		{
			glm::mat4 model;
			float rotate;
			glm::vec3 axis;
		};
		struct LightParam
		{
			glm::vec3 position;
			glm::vec3 ambientColor;
			glm::vec3 diffuseColor;
			glm::vec3 specularColor;
			glm::vec3 strengths;
			glm::vec3 attenue;
		};
	public:
		IDrawer();
		virtual ~IDrawer();
		virtual void init(const Config & config) = 0;
		virtual void update(float delta) = 0;
		virtual void draw() = 0;
		virtual void release() = 0;
		virtual std::shared_ptr<RenderTarget> renderTarget() = 0;
	};
}

