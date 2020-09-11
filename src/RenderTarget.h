#pragma once

#include "common.h"

#include "FrameBufferObject.h"

namespace sunty
{
	class RenderTarget
	{
	public:
		struct Options
		{
			bool externalFBO;
			GLuint externalFBOID;
			int viewX, viewY, viewW, viewH;
			glm::vec4 clearColor;
			float clearDepth;
			int textureCount;
			Texture::Option textureOptions[8];
			bool hasDepth;
			Texture::Option depthOption;
		};
	public:
		RenderTarget();
		~RenderTarget();

		void setup(const Options & options);
		void release();
		void push();
		void clear();
		
		std::shared_ptr<FrameBufferObject> fbo()
		{
			return mFBO;
		}
		std::vector<std::shared_ptr<Texture>> & textures()
		{
			return mTextures;
		}
		std::shared_ptr<Texture> texture(int index)
		{
			return mTextures[index];
		}
		Options & options()
		{
			return mOptions;
		}
	private:
		Options mOptions;
		std::shared_ptr<FrameBufferObject> mFBO;
		std::shared_ptr<Texture> mDepthTexture;
		std::vector<std::shared_ptr<Texture>> mTextures;
		std::vector<GLenum> mMRT;
	};
}
