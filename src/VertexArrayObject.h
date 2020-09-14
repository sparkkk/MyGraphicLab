#pragma once

#include "common.h"

namespace sunty
{
	class VertexArrayObject
	{
	public:
		VertexArrayObject();
		~VertexArrayObject();
		void setup(const int * indices, size_t count);
		void release();
		void use(bool value);
	private:
		GLuint mVAO = 0;
		GLuint mEBO = 0;
		int mElementCount = 0;
	public:
		void drawTriangles();
		void drawTriangleStrip();
	};
}


