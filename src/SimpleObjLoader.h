#pragma once

#include "common.h"
#include "VertexBufferObject.h"
#include "VertexArrayObject.h"

namespace sunty
{
	class SimpleObjLoader
	{
	public:
		SimpleObjLoader();
		~SimpleObjLoader();
		bool load(
			const char * path,
			bool calcTangent,
			VertexBufferObject & vbo,
			VertexArrayObject & vao
		);
	};

}
