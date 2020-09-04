#include "VertexArrayObject.h"

using namespace sunty;

VertexArrayObject::VertexArrayObject()
{
}


VertexArrayObject::~VertexArrayObject()
{
	release();
}


void VertexArrayObject::pushIndices(int count, int * indices)
{
	mIndices.assign(indices, indices + count);
}


void VertexArrayObject::setup()
{
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * mIndices.size(), mIndices.data(), GL_STATIC_DRAW);
	mElementCount = mIndices.size();
	mIndices.clear();
}


void VertexArrayObject::release()
{
	if (mEBO != 0)
	{
		glDeleteBuffers(1, &mEBO);
		mEBO = 0;
	}
	if (mVAO != 0)
	{
		glDeleteVertexArrays(1, &mVAO);
		mVAO = 0;
	}
	mIndices.clear();
	mElementCount = 0;
}


void VertexArrayObject::use(bool value)
{
	if (value)
	{
		glBindVertexArray(mVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	}
	else
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}


void VertexArrayObject::drawTriangles()
{
	glDrawElements(GL_TRIANGLES, mElementCount, GL_UNSIGNED_INT, NULL);
}

void VertexArrayObject::drawTriangleStrip()
{
	glDrawElements(GL_TRIANGLE_STRIP, mElementCount, GL_UNSIGNED_INT, NULL);
}
