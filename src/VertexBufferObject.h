#pragma once

#include "common.h"

namespace sunty
{
	class VertexBufferObject
	{
	public:
		enum AttrType
		{
			ATTR_POSITION,
			ATTR_TEXTURE_COORD,
			ATTR_NORMAL,
			ATTR_TANGENT,
			ATTR_COLOR,
		};
		struct AttrDesc
		{
			AttrType type;
			int columns = 0;
			std::vector<float> data;
			AttrDesc(AttrType type, int columns)
			{
				this->type = type;
				this->columns = columns;
			}
		};
	public:
		VertexBufferObject();
		~VertexBufferObject();
		void pushAttr(AttrType attrType, int columns);
		void pushData(AttrType attrType, int rows, const float * data);
		void setup();
		void setup(const float * data, uint32_t size);
		void release();
		void use(bool value);
		int columns() { return mColumns; }
		const std::vector<AttrDesc> & attrDescs() { return mAttrDescs; }
		const AttrDesc * attrDesc(AttrType attrType);
	private:
		int mColumns = 0;
		std::vector<AttrDesc> mAttrDescs;
		GLuint mVBO = 0;
	};
}


