#pragma once

#include "common.h"
#include "IRenderObject.h"
#include "Mesh.h"
#include "Shader.h"
#include "UniformValue.h"

namespace sunty
{
	class RenderObject : public IRenderObject
	{
	public:
		RenderObject() = default;
		virtual ~RenderObject() = default;
		void setup(
			std::shared_ptr<Mesh> mesh,
			std::shared_ptr<Shader> shader,
			RenderOptions options
		);
		virtual void setParam(const char * name, const UniformValue & value) override;
		virtual void draw() override;
	private:
		std::shared_ptr<Mesh> mMesh;
		std::unordered_map<std::string, UniformValue> mParams;
		std::shared_ptr<Shader> mShader;
		RenderOptions mOptions;
	};
}

