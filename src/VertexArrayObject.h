#pragma once

#include "common.h"

namespace sunty
{
	class VertexArrayObject
	{
	public:
		VertexArrayObject();
		~VertexArrayObject();
		void pushIndices(int count, int* indices);
		void setup();
		void release();
		void use(bool value);
	private:
		GLuint mVAO = 0;
		GLuint mEBO = 0;
		std::vector<int> mIndices;
		int mElementCount = 0;
	public:
		void drawTriangles();
		void drawTriangleStrip();
	};
}


