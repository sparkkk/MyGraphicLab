#pragma once

#include "common.h"
#include "Texture.h"

namespace sunty
{
	class FrameBufferObject
	{
	public:
		FrameBufferObject();
		~FrameBufferObject();
		void setup();
		void setup(GLuint externalID);
		void release();
		void use(bool value);
	private:
		bool mExternalID = false;
		GLuint mFBO = 0;
	public:
		void attachColor(Texture & texture, int layout);
		void attachDepth(Texture & texture);
	};
}


