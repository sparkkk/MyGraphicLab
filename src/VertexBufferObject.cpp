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

void VertexBufferObject::pushData(AttrType attrType, int rows, const float * data)
{
	for (auto & attrDesc : mAttrDescs)
	{
		if (attrDesc.type != attrType)
		{
			continue;
		}
		attrDesc.data.insert(
			attrDesc.data.end(),
			data,
			data + rows * attrDesc.columns
		);
		break;
	}
}


void VertexBufferObject::setup()
{
	int rows = 0;
	for (auto & attrDesc : mAttrDescs)
	{
		int r = attrDesc.data.size() / attrDesc.columns;
		rows = rows == 0 ? r : (r < rows ? r : rows);
	}
	std::vector<float> data;
	for (int i = 0; i < rows; ++i)
	{
		for (auto & attrDesc : mAttrDescs)
		{
			data.insert(
				data.end(),
				attrDesc.data.begin() + i * attrDesc.columns,
				attrDesc.data.begin() + (i+1) * attrDesc.columns
			);
		}
	}
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);
	for (auto & attrDesc : mAttrDescs)
	{
		attrDesc.data.clear();
	}
}

void sunty::VertexBufferObject::setup(const float * data, uint32_t size)
{
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, data, GL_STATIC_DRAW);
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
