#pragma once

#include "common.h"
#include "Texture.h"

namespace sunty
{
	struct UniformValue
	{
		enum Type
		{
			TYPE_SCALAR,
			TYPE_VECTOR2,
			TYPE_VECTOR3,
			TYPE_VECTOR4,
			TYPE_MATRIX3,
			TYPE_MATRIX4,
			TYPE_TEXTURE,
			TYPE_INTEGER,
		};

		Type type;
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
	};
}