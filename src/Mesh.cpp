#include "Mesh.h"

using namespace sunty;

void Mesh::setup(const float * data, size_t dataCount, const int * indices, size_t indexCount)
{
    VertexBufferObject::setup(data, dataCount);
    VertexArrayObject::setup(indices, indexCount);
}

void Mesh::release()
{
    VertexArrayObject::release();
    VertexBufferObject::release();
}

void Mesh::use(bool value)
{
    if (value)
    {
        VertexArrayObject::use(value);
        VertexBufferObject::use(value);
    }
    else
    {
        VertexBufferObject::use(value);
        VertexArrayObject::use(value);
    }
}