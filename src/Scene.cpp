#include "Scene.h"

using namespace sunty;

Scene::Scene()
{
}


Scene::~Scene()
{
}

void Scene::setup()
{
	update(0);

	Camera * pCamera = nullptr;

	for (auto & c : cameras)
	{
		if (c.pass & pass)
		{
			pCamera = &c;
			break;
		}
	}
	if (pCamera == nullptr)
	{
		return;
	}

	auto flipper = glm::mat4(1.0);
	auto & camera = *pCamera;
	if (camera.flipY)
	{
		flipper = glm::scale(glm::mat4(1.0), glm::vec3(1, -1, 1));
	}

	for (int k = 0; k < renders.size(); ++k)
	{
		auto & render = renders[k];
		if ((render->getPassMask() & pass) == 0)
		{
			continue;
		}
		auto & transform = matrices[k];
		bool follow = follows[k];

		auto mvp = flipper * camera.getVP() * transform;
		if (follow)
		{
			auto mv = glm::mat4(glm::mat3(camera.getV() * transform));
			mvp = flipper * camera.getP() * mv;
		}

		render->setParam("PositionTransform", mvp);
		render->setParam("WorldTransform", transform);
		render->setParam("ViewerPosition", camera.position);
		render->setParam("Light.LightCount", (int) lights.size());

		for (int i = 0; i < lights.size(); ++i)
		{
			std::stringstream ss;
			ss << '[' << i << ']';
			std::string indexNote = ss.str();

			for (auto & entry : lights[i].uniforms)
			{
				auto id = entry.first;
				auto & value = entry.second;

				auto p = id.find("[n]");
				if (p != std::string::npos)
				{
					id = id.replace(p, 3, indexNote);
				}
				render->setParam(id.c_str(), value);
			}
		}
	}
}

static float _evalExpr(
	const std::unordered_map<std::string, float> & valueMap,
	const std::vector<std::string> & symbols,
	int & index)
{
	auto & symbol = symbols[index];
	if (symbol == "*" || symbol == "+")
	{
		++index;
		float v1 = _evalExpr(valueMap, symbols, index);
		float v2 = _evalExpr(valueMap, symbols, index);
		if (symbol == "*")
		{
			return v1 * v2;
		}
		else if (symbol == "+")
		{
			return v1 + v2;
		}
	}
	else if (valueMap.find(symbol) != valueMap.end())
	{
		++index;
		return valueMap.at(symbol);
	}
	else
	{
		++index;
		return std::atof(symbol.c_str());
	}
	return 0;
}


static float evalExpr(
	const std::unordered_map<std::string, float> & valueMap,
	const std::vector<std::string> & symbols)
{
	int index = 0;
	return _evalExpr(valueMap, symbols, index);
}

void Scene::update(float delta)
{
	matrices.clear();
	valueMap["delta"] = delta;
	for (auto & renderTransform : this->transforms)
	{
		glm::mat4 transformMat(1.0f);
		for (auto & transform : renderTransform)
		{
			if (transform.type == Transform::TYPE_SCALE)
			{
				float v1 = evalExpr(valueMap, transform.exprs[0]);
				float v2 = evalExpr(valueMap, transform.exprs[1]);
				float v3 = evalExpr(valueMap, transform.exprs[2]);
				transformMat = glm::scale(glm::mat4(1.0f), glm::vec3(v1, v2, v3)) * transformMat;
			}
			else if (transform.type == Transform::TYPE_ROTATE)
			{
				float v1 = evalExpr(valueMap, transform.exprs[0]);
				float v2 = evalExpr(valueMap, transform.exprs[1]);
				float v3 = evalExpr(valueMap, transform.exprs[2]);
				float v4 = evalExpr(valueMap, transform.exprs[3]);
				transformMat = glm::rotate(glm::mat4(1.0f), v1 * glm::pi<float>() / 180.0f, glm::vec3(v2, v3, v4)) * transformMat;
			}
			else if (transform.type == Transform::TYPE_TRANSLATE)
			{
				float v1 = evalExpr(valueMap, transform.exprs[0]);
				float v2 = evalExpr(valueMap, transform.exprs[1]);
				float v3 = evalExpr(valueMap, transform.exprs[2]);
				transformMat = glm::translate(glm::mat4(1.0f), glm::vec3(v1, v2, v3)) * transformMat;
			}
		}
		matrices.emplace_back(transformMat);
	}

	Camera * pCamera = nullptr;

	for (auto & c : cameras)
	{
		if (c.pass & pass)
		{
			pCamera = &c;
			break;
		}
	}
	if (pCamera == nullptr)
	{
		return;
	}

	auto flipper = glm::mat4(1.0);
	auto & camera = *pCamera;
	if (camera.flipY)
	{
		flipper = glm::scale(glm::mat4(1.0), glm::vec3(1, -1, 1));
	}

	for (int k = 0; k < renders.size(); ++k)
	{
		auto & render = renders[k];
		if ((render->getPassMask() & pass) == 0)
		{
			continue;
		}
		auto & transform = matrices[k];
		bool follow = follows[k];

		auto mvp = flipper * camera.getVP() * transform;
		if (follow)
		{
			auto mv = glm::mat4(glm::mat3(camera.getV() * transform));
			mvp = flipper * camera.getP() * mv;
		}
		render->setParam("PositionTransform", mvp);
		render->setParam("WorldTransform", transform);
	}
}

void Scene::draw()
{
	for (auto & render : renders)
	{
		if ((render->getPassMask() & pass) == 0)
		{
			continue;
		}
		render->draw();
	}
}

void Scene::setCurrentPass(Pass pass)
{
	for (auto r : renders)
	{
		r->setCurrentPass(pass);
	}
	this->pass = pass;
}