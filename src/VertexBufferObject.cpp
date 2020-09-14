#include "VertexBufferObject.h"

using namespace sunty;

VertexBufferObject::VertexBufferObject()
{
}


VertexBufferObject::~VertexBufferObject()
{
	release();
}


void VertexBufferObject::pushAttr(AttrType attrType, int columns)
{
	mAttrDescs.emplace_back(attrType, columns);
	mColumns += columns;
}

void sunty::VertexBufferObject::setup(const float * data, uint32_t count)
{
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count, data, GL_STATIC_DRAW);
}

void VertexBufferObject::release()
{
	if (mVBO != 0)
	{
		glDeleteBuffers(1, &mVBO);
		mVBO = 0;
		mAttrDescs.clear();
		mColumns = 0;
	}
}


void VertexBufferObject::use(bool value)
{
	glBindBuffer(GL_ARRAY_BUFFER, value ? mVBO : 0);
}

const VertexBufferObject::AttrDesc * VertexBufferObject::attrDesc(AttrType attrType)
{
	for (auto & ad : mAttrDescs)
	{
		if (ad.type == attrType)
		{
			return &ad;
		}
	}
	return nullptr;
}
