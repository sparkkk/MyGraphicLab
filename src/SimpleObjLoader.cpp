#include "SimpleObjLoader.h"
#include <fstream>
#include <sstream>
#include <chrono>

using namespace sunty;

typedef struct {
	glm::vec3 v;
	glm::vec2 vt;
	glm::vec3 vn;
	glm::vec3 vg;
} AlignedVertex;
typedef struct { uint32_t i[3]; } Face;

SimpleObjLoader::SimpleObjLoader()
{
}


SimpleObjLoader::~SimpleObjLoader()
{
}

static std::vector<std::string> split(const std::string & text, const std::string & sep)
{
	std::vector<std::string> splits;
	std::size_t offset = 0;
	auto pos = text.find(sep, offset);
	while (pos != std::string::npos)
	{
		if (pos - offset > 0)
		{
			splits.emplace_back(text, offset, pos - offset);
		}
		offset = pos + sep.size();
		pos = text.find(sep, offset);
	}
	if (offset < text.size())
	{
		splits.emplace_back(text, offset);
	}
	return splits;
}

static glm::vec3 _calcTangent(
	const AlignedVertex & va, 
	const AlignedVertex & vb,
	const AlignedVertex & vc)
{
	glm::vec3 Eab = vb.v - va.v;
	glm::vec3 Eac = vc.v - va.v;
	glm::vec2 Tab = vb.vt - va.vt;
	glm::vec2 Tac = vc.vt - va.vt;
	glm::mat3 Tm = glm::transpose(glm::mat3(
		glm::vec3(Tab, 0),
		glm::vec3(Tac, 0),
		glm::vec3(0, 0, 1)
	));
	float det = glm::determinant(Tm);
	if (det < 0.000001f && det > -0.000001f)
	{
		return glm::vec3(0, 0, 0);
	}
	glm::mat3 Im = glm::inverse(Tm);
	glm::mat3 Em = glm::transpose(glm::mat3(
		Eab,
		Eac,
		glm::vec3(0, 0, 1)
	));
	glm::mat3 TBm = Im * Em;
	return glm::row(TBm, 0);
}

bool sunty::SimpleObjLoader::load(
	const std::filesystem::path & path,
	bool calcTangent,
	VertexBufferObject & vbo, 
	VertexArrayObject & vao)
{
	std::ifstream stream;

	stream.open(path);

	if (!stream.is_open())
	{
		return false;
	}

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;

	vertices.emplace_back(0, 0, 0);
	texCoords.emplace_back(0, 0);
	normals.emplace_back(0, 0, 0);

	//printf("AlignedVertex size=%d\n", sizeof(AlignedVertex));
	//printf("Face size=%d\n", sizeof(Face));
	std::vector<AlignedVertex> alignedVertices;
	std::unordered_map<std::string, uint32_t> indicesMap;
	std::vector<Face> faces;

	int stage = 0;
	auto t0 = std::chrono::steady_clock::now();

	std::string line;
	do
	{
		std::getline(stream, line);
		if (line[0] == '#')
		{
			continue;
		}
		std::vector<std::string> splits = split(line, " ");
		if (splits.size() == 0)
		{
			continue;
		}
		auto & head = splits[0];
		if (head == "v" && splits.size() >= 4)
		{
			if (stage == 0)
			{
				stage = 1;
			}
			vertices.emplace_back(
				std::atof(splits[1].c_str()),
				std::atof(splits[2].c_str()),
				std::atof(splits[3].c_str())
			);
		}
		else if (head == "vt" && splits.size() >= 3)
		{
			if (stage == 1)
			{
				stage = 2;
			}
			texCoords.emplace_back(
				std::atof(splits[1].c_str()),
				std::atof(splits[2].c_str())
			);
		}
		else if (head == "vn" && splits.size() >= 4)
		{
			if (stage == 2)
			{
				stage = 3;
			}
			normals.emplace_back(
				std::atof(splits[1].c_str()),
				std::atof(splits[2].c_str()),
				std::atof(splits[3].c_str())
			);
		}
		else if (head == "f" && splits.size() >= 4)
		{
			if (stage == 3)
			{
				auto t1 = std::chrono::steady_clock::now();
				auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
				//printf("stage 1~3 cost %lld milli-sec\n", dur.count());
				t0 = t1;
				stage = 4;
			}
			for (int i = 1; i < 4; ++i)
			{
				auto & vcnStr = splits[i];
				if (indicesMap.find(vcnStr) == indicesMap.end())
				{
					auto vcn = split(vcnStr, "/");
					int vi = 0;
					int ci = 0;
					int ni = 0;
					if (vcn.size() >= 1)
					{
						vi = std::atoi(vcn[0].c_str());
					}
					if (vcn.size() >= 2)
					{
						ci = std::atoi(vcn[1].c_str());
					}
					if (vcn.size() >= 3)
					{
						ni = std::atoi(vcn[2].c_str());
					}
					if (vi < 0)
					{
						vi = vertices.size() + vi;
					}
					if (ci < 0)
					{
						ci = texCoords.size() + ci;
					}
					if (ni < 0)
					{
						ni = normals.size() + ni;
					}
					alignedVertices.emplace_back();
					alignedVertices.back().v = vertices[vi];
					alignedVertices.back().vt = texCoords[ci];
					alignedVertices.back().vn = normals[ni];
					alignedVertices.back().vg = glm::vec3(0.0f);

					indicesMap[vcnStr] = alignedVertices.size() - 1;
				}
			}
			faces.emplace_back();
			faces.back().i[0] = indicesMap[splits[1]];
			faces.back().i[1] = indicesMap[splits[2]];
			faces.back().i[2] = indicesMap[splits[3]];

			if (calcTangent)
			{
				int ia = faces.back().i[0];
				int ib = faces.back().i[1];
				int ic = faces.back().i[2];
				auto & va = alignedVertices[ia];
				auto & vb = alignedVertices[ib];
				auto & vc = alignedVertices[ic];
				auto ta = _calcTangent(va, vb, vc);
				auto tb = _calcTangent(vb, vc, va);
				auto tc = _calcTangent(vc, va, vb);
				va.vg = ta;
				vb.vg = tb;
				vc.vg = tc;
			}
		}
	} while (!stream.eof());

	stream.close();

	if (stage == 4)
	{
		auto t1 = std::chrono::steady_clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
		//printf("stage 4 cost %lld milli-sec\n", dur.count());
		t0 = t1;
		stage = 5;
	}

	vertices.clear();
	texCoords.clear();
	normals.clear();
	indicesMap.clear();

	// for (int k = 0; k < alignedVertices.size(); ++k)
	// {
	// 	printf("%d: %f %f %f\n", k, alignedVertices[k].v.x, alignedVertices[k].v.y, alignedVertices[k].v.z);
	// }
	// for (int k = 0; k < faces.size(); ++k)
	// {
	// 	printf("%d %d %d\n", faces[k].i[0], faces[k].i[1], faces[k].i[2]);
	// }

	vbo.pushAttr(VertexBufferObject::AttrType::ATTR_POSITION, 3);
	vbo.pushAttr(VertexBufferObject::AttrType::ATTR_TEXTURE_COORD, 2);
	vbo.pushAttr(VertexBufferObject::AttrType::ATTR_NORMAL, 3);
	vbo.pushAttr(VertexBufferObject::AttrType::ATTR_TANGENT, 3);

	const float * buffer = (const float *)alignedVertices.data();
	vbo.setup(
		buffer,
		alignedVertices.size() * (sizeof(alignedVertices[0]) / sizeof(float))
	);
	vao.pushIndices(
		faces.size() * (sizeof(faces[0]) / sizeof(uint32_t)),
		(int *)faces.data()
	);
	vao.setup();
	printf("obj model %s loaded successfully\n", path.string().c_str());
	return true;
}
