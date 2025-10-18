#include "Camera.h"

glm::mat4 getProjectByType(CameraType type, Frustrum& frustrum)
{
	glm::mat4 projection = glm::mat4(1.0f);
	switch (type)
	{
		case CameraType::Perspective:
		{
			projection = glm::perspective(glm::radians(frustrum.fov), (float) frustrum.x / (float) frustrum.y, frustrum.near, frustrum.far);
			break;
		}
		case CameraType::Orthographic:
		{
			float halfX = frustrum.x/2;
			float halfY = frustrum.y/2;
			projection = glm::ortho(-halfX, halfX, -halfY, halfY, frustrum.near, frustrum.far);
			break;
		}
	}

	return projection;
}

Camera::Camera()
{
	name = "Camera";

	frustrum.fov = 45.0f;
	frustrum.position = glm::vec3(0.0f,2.0f,-5.0f);
	frustrum.target = glm::vec3(0.0f, 0.0f, 0.0f);
	frustrum.up = glm::vec3(0.0, 1.0, 0.0f);
	frustrum.x = 800;
	frustrum.y = 800;

	type = CameraType::Perspective;

	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);

	view = glm::lookAt(frustrum.position, frustrum.target, frustrum.up);

	projection = getProjectByType(type, frustrum);
	viewProjection = projection * view;
}

Camera::Camera(const std::string& nme, const Frustrum& frust)
{
	name = nme;

	frustrum.fov = frust.fov;
	frustrum.position = frust.position;
	frustrum.target = frust.target;
	frustrum.up = frust.up;
	frustrum.x = frust.x;
	frustrum.y = frust.y;
	frustrum.near = 1.0f;
	frustrum.far = 10000.0f;

	type = CameraType::Perspective;

	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);

	view = glm::lookAt(frustrum.position, frustrum.target, frustrum.up);

	projection = getProjectByType(type,frustrum);
	viewProjection = projection * view;
}

Camera::Camera(const std::string& nme, glm::vec3 position, float x, float y)
{
	name = nme;

	frustrum.fov = 45.0f;
	frustrum.position = position;
	frustrum.target = glm::vec3(0.0f, 0.0f, -1.0f);
	frustrum.up = glm::vec3(0.0, 1.0, 0.0f);
	frustrum.x = x;
	frustrum.y = y;

	type = CameraType::Perspective;


	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);

	view = glm::lookAt(frustrum.position, frustrum.target, frustrum.up);

	projection = getProjectByType(type,frustrum);
	viewProjection = projection * view;
}

Camera::~Camera()
{

}

void Camera::recalculate()
{
	projection = getProjectByType(type, frustrum);
	viewProjection = projection * view;
}

void Camera::setCamera(CameraType cameraType)
{
	type = cameraType;
	projection = getProjectByType(cameraType,frustrum);
	viewProjection = projection * view;
}

void Camera::setName(const std::string& nme)
{
	name = nme;
}

void Camera::setAspectRatio(float x, float y)
{
	frustrum.x = x;
	frustrum.y = y;
	recalculate();
}

void Camera::setPosition(glm::vec3 position)
{
	frustrum.position = position;
	view = glm::lookAt(position, frustrum.target, frustrum.up);
	viewProjection = projection * view;
}

void Camera::setTarget(glm::vec3 target)
{
	frustrum.target = target;
	view = glm::lookAt(frustrum.position, target, frustrum.up);
	viewProjection = projection * view;
}

void Camera::setNearPlane(float nearPlane)
{
	frustrum.near = nearPlane;
	projection = getProjectByType(type,frustrum);
	viewProjection = projection * view;
}

void Camera::setFarPlane(float farPlane)
{
	frustrum.far = farPlane;
	projection = getProjectByType(type, frustrum);
	viewProjection = projection * view;
}

void Camera::setFOV(float FOV)
{
	frustrum.fov = FOV;
	projection = getProjectByType(type, frustrum);
	viewProjection = projection * view;
}

void Camera::setType(CameraType typ)
{
	type = typ;
	recalculate();
}

void Camera::updatePosition(glm::vec3 position)
{
	frustrum.position += position;
	view = glm::lookAt(frustrum.position, frustrum.target, frustrum.up);
	viewProjection = projection * view;
}

void Camera::updateTarget(glm::vec3 target)
{
	frustrum.target += target;
	view = glm::lookAt(frustrum.position, frustrum.target, frustrum.up);
	viewProjection = projection * view;
}

glm::vec3 Camera::getRightVector() const
{
	return glm::transpose(view)[0];
}

glm::vec3 Camera::getViewDirection() const
{
	return -glm::transpose(view)[2];
}

glm::mat4 Camera::getViewProject() const
{
	return viewProjection;
}

glm::mat4 Camera::getView()
{
	return view;
}

glm::mat4 Camera::getProjection()
{
	return projection;
}

const Frustrum& Camera::getFrustrum() const
{
	return frustrum;
}

Frustrum& Camera::getFrustrum()
{
	return frustrum;
}

const CameraType& Camera::getCameraType() const
{
	return type;
}

const std::string& Camera::getName() const
{
	return name;
}

std::string& Camera::getName()
{
	return name;
}

std::string getCameraTypeString(CameraType type)
{
	switch (type)
	{
		case CameraType::Perspective:
			return "Perpspective";
		case CameraType::Orthographic:
			return "Orthographic";
		default: return "Perspective";
	}
}