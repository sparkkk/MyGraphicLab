#pragma once

#include "common.h"

#include "VertexBufferObject.h"
#include "VertexArrayObject.h"
#include "Shader.h"
#include "UniformValue.h"

namespace sunty
{
	struct RenderOptions
	{
		enum
		{
			DRAW_LIST = 0,
			DRAW_STRIP,
			DRAW_FAN
		} drawMode;
		enum
		{
			CULL_NONE,
			CULL_BACK,
			CULL_FRONT
		} cullMode;
		bool alphaBlend;
		bool depthTest;
		bool depthMask;
	};
	class RenderObject
	{
	public:
		RenderObject();
		~RenderObject();
		std::shared_ptr<VertexBufferObject> vbo()
		{
			return mVBO;
		}
		std::shared_ptr<VertexArrayObject> vao()
		{
			return mVAO;
		}
		std::shared_ptr<Shader> shader()
		{
			return mShader;
		}
		RenderOptions & options()
		{
			return mOptions;
		}
		void setup(
			std::shared_ptr<VertexBufferObject> vbo,
			std::shared_ptr<VertexArrayObject> vao,
			std::shared_ptr<Shader> shader,
			RenderOptions options
		);
		bool setParam(const char * name, float value);
		bool setParam(const char * name, int value);
		bool setParam(const char * name, const glm::vec2 & value);
		bool setParam(const char * name, const glm::vec3 & value);
		bool setParam(const char * name, const glm::vec4 & value);
		bool setParam(const char * name, const glm::mat3 & value);
		bool setParam(const char * name, const glm::mat4 & value);
		bool setParam(const char * name, std::shared_ptr<Texture> value);
		bool setParam(const char * name, const UniformValue & value);
		void draw();
		void release();
		Pass pass = PASS_DEFAULT;
	private:
		std::shared_ptr<VertexBufferObject> mVBO;
		std::shared_ptr<VertexArrayObject> mVAO;
		std::unordered_map<std::string, UniformValue> mParams;
		std::shared_ptr<Shader> mShader;
		RenderOptions mOptions;
	};
}

