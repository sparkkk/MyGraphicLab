#include "RenderTarget.h"

using namespace sunty;

RenderTarget::RenderTarget()
{
}


RenderTarget::~RenderTarget()
{
	release();
}

void sunty::RenderTarget::setup(const Options & options)
{
	mOptions = options;

	mFBO = std::make_shared<FrameBufferObject>();

	if (mOptions.externalFBO)
	{
		mFBO->setup(mOptions.externalFBOID);
	}
	else
	{
		mFBO->setup();
	}
	mFBO->use(true);
	std::vector<GLenum> mrt;
	for (int i = 0; i < mOptions.textureCount; ++i)
	{
		auto tex = std::make_shared<Texture>();
		tex->setup(mOptions.textureOptions[i]);
		mFBO->attachColor(*tex, i);
		mTextures.emplace_back(tex);
		mrt.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	mMRT = std::move(mrt);
	if (mOptions.hasDepth)
	{
		this->mDepthTexture = std::make_shared<Texture>();
		this->mDepthTexture->setup(mOptions.depthOption);
		mFBO->attachDepth(*this->mDepthTexture);
	}
	mFBO->use(false);
}

void sunty::RenderTarget::release()
{
}

void sunty::RenderTarget::push()
{
	mFBO->use(true);
	glClearColor(
		mOptions.clearColor.r,
		mOptions.clearColor.g,
		mOptions.clearColor.b,
		mOptions.clearColor.a
	);
	glClearDepth(mOptions.clearDepth);
	glViewport(
		mOptions.viewX,
		mOptions.viewY,
		mOptions.viewW,
		mOptions.viewH
	);
	if (mMRT.size() > 0)
	{
		glDrawBuffers(mMRT.size(), mMRT.data());
	}
}

void sunty::RenderTarget::clear()
{
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

