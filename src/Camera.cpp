#include "Camera.h"

using namespace sunty;

Camera::Camera()
{
	mMatView = glm::mat4(1.0);
	mMatProj = glm::mat4(1.0);
	mMatVP = glm::mat4(1.0);
	position = glm::vec3(0.0);
	flipY = false;
}


Camera::~Camera()
{
}

void Camera::reset()
{
	mMatView = glm::mat4(1.0);
	mMatProj = glm::mat4(1.0);
	mMatVP = glm::mat4(1.0);
	position = glm::vec3(0.0);
	flipY = false;
}

//ASSUME MODLE COORDINATE AND VIEW COORDINATE ARE BOTH RIGHT-HANDED
void Camera::makeView(const glm::vec3 & at, const glm::vec3 & look, const glm::vec3 & up)
{
	glm::vec3 A = look - at;
	glm::vec3 B = up + at;
	float alpha = glm::dot(A, B) / glm::dot(A, A);
	glm::vec3 z = glm::normalize(A);
	glm::vec3 y = glm::normalize(B - alpha * A);
	glm::vec3 x = glm::normalize(glm::cross(z, y));
	glm::mat4 matCamera = glm::mat4(
		glm::vec4(x, 0),
		glm::vec4(y, 0),
		glm::vec4(z, 0),
		glm::vec4(at, 1.0)
	);
	mMatView = glm::inverse(matCamera);
	mMatVP = mMatProj * mMatView;
	position = at;

	//auto tt = mMatView * glm::vec4(1,1,1,1);
	//printf("test view: %f %f %f %f\n", tt.x, tt.y, tt.z, tt.w);
}


void Camera::makeOrthoProj(float width, float height, float near, float far)
{
	auto m = glm::mat4(0.0);
	float * p = glm::value_ptr(m);
	p[0] = 2.0 / width;
	p[5] = 2.0 / height;
	p[10] = 2.0 / (far - near);
	p[14] = -(far + near) / (far - near);
	p[15] = 1.0;
	mMatProj = m;
	mMatVP = mMatProj * mMatView;
}


void Camera::makePerspectProj(float fanAngle, float aspect, float near, float far)
{
	auto m = glm::mat4(0.0);
	auto p = glm::value_ptr(m);
	p[0] = 1.0 / glm::tan(fanAngle * 0.5);
	p[5] = p[0] * aspect;
	p[10] = (far + near) / (far - near);
	p[11] = 1.0;
	p[14] = -2.0 * far * near / (far - near);
	mMatProj = m;
	mMatVP = mMatProj * mMatView;
}

