#pragma once

#include "common.h"
#include "Texture.h"

namespace sunty
{
    struct UniformValue
    {
        enum Type
        {
            TYPE_NONE = 0,
            TYPE_SCALAR,
            TYPE_VECTOR2,
            TYPE_VECTOR3,
            TYPE_VECTOR4,
            TYPE_MATRIX3,
            TYPE_MATRIX4,
            TYPE_TEXTURE,
            TYPE_INTEGER,
        };

        Type type = TYPE_NONE;
        union
        {
            float scalar;
            float vector2[2];
            float vector3[3];
            float vector4[4];
            float matrix3[9];
            float matrix4[16];
            int integer;
        };
        std::shared_ptr<Texture> texture;

        UniformValue() = default;

        UniformValue(const UniformValue & other) = default;

        UniformValue(UniformValue && other) = default;

        UniformValue(float value)
        {
            type = TYPE_SCALAR;
            scalar = value;
        }
        UniformValue(int value)
        {
            type = TYPE_INTEGER;
            integer = value;
        }
        UniformValue(const glm::vec2 & value)
        {
            type = TYPE_VECTOR2;
            memcpy(vector2, glm::value_ptr(value), sizeof(vector2));
        }
        UniformValue(const glm::vec3 & value)
        {
            type = TYPE_VECTOR3;
            memcpy(vector3, glm::value_ptr(value), sizeof(vector3));
        }
        UniformValue(const glm::vec4 & value)
        {
            type = TYPE_VECTOR4;
            memcpy(vector4, glm::value_ptr(value), sizeof(vector4));
        }
        UniformValue(const glm::mat3 & value)
        {
            type = TYPE_MATRIX3;
            memcpy(matrix3, glm::value_ptr(value), sizeof(matrix3));
        }
        UniformValue(const glm::mat4 & value)
        {
            type = TYPE_MATRIX4;
            memcpy(matrix4, glm::value_ptr(value), sizeof(matrix4));
        }
        UniformValue(std::shared_ptr<Texture> value)
        {
            type = TYPE_TEXTURE;
            texture = value;
        }

        UniformValue & operator = (const UniformValue & other) = default;
    };
}