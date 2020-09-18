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
			bool externalFBO = false;
			GLuint externalFBOID = 0;
			int viewX = 0, viewY = 0, viewW = 0, viewH = 0;
			glm::vec4 clearColor = glm::vec4(0,0,0,1);
			float clearDepth = 1.0f;
			int textureCount = 0;
			Texture::Option textureOptions[8];
			bool hasDepth = false;
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
		std::shared_ptr<Texture> depthTexture()
		{
			return mDepthTexture;
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
