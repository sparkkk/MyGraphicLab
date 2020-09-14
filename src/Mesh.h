#pragma once
#include "common.h"
#include "IResource.h"
#include "VertexBufferObject.h"
#include "VertexArrayObject.h"

namespace sunty
{
    class Mesh : public IResource, public VertexBufferObject, public VertexArrayObject
    {
	public:
		Mesh() = default;
		~Mesh() = default;
		void setup(const float * data, size_t dataCount, const int * indices, size_t indexCount);
		void release();
		void use(bool value);
    };
}