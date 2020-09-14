#pragma once

#include "common.h"
#include "Mesh.h"

namespace sunty
{
	class SimpleObjLoader
	{
	public:
		SimpleObjLoader();
		~SimpleObjLoader();
		bool load(
			const std::filesystem::path & path,
			bool calcTangent,
			Mesh & mesh
		);
	};

}
