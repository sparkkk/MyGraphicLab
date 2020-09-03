#pragma once

#include "common.h"

namespace sunty
{
	class Camera
	{
	public:
		Camera();
		~Camera();
		void makeView(const glm::vec3 & at, const glm::vec3 & look, const glm::vec3 & up);
		void makeOrthoProj(float width, float height, float near, float far);
		void makePerspectProj(float fanAngle, float aspect, float near, float far);
		void reset();
		glm::mat4 getVP() { return mMatVP; }
		glm::mat4 getV() { return mMatView; }
		glm::mat4 getP() { return mMatProj; }
		glm::vec3 position;
		bool flipY;
	private:
		glm::mat4 mMatView;
		glm::mat4 mMatProj;
		glm::mat4 mMatVP;
	};
}

