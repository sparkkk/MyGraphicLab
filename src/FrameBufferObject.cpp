#include "FrameBufferObject.h"


using namespace sunty;

FrameBufferObject::FrameBufferObject()
{
}


FrameBufferObject::~FrameBufferObject()
{
	release();
}


void FrameBufferObject::setup()
{
	mExternalID = false;
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
}

void sunty::FrameBufferObject::setup(GLuint externalID)
{
	mExternalID = true;
	mFBO = externalID;
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
}


void FrameBufferObject::release()
{
	if (!mExternalID && mFBO != 0)
	{
		glDeleteFramebuffers(1, &mFBO);
		mFBO = 0;
	}
}


void FrameBufferObject::use(bool value)
{
	glBindFramebuffer(GL_FRAMEBUFFER, value ? mFBO : 0);
}


void FrameBufferObject::attachColor(Texture & texture, int layout)
{
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + layout,
		GL_TEXTURE_2D,
		texture.id(),
		0
	);
}

void sunty::FrameBufferObject::attachDepth(Texture & texture)
{
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		texture.id(),
		0
	);
}
