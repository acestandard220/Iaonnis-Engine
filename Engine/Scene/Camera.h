#pragma once
#include "../Core/pch.h"

struct Frustrum
{
	float near = 1.0f;
	float far = 10000.0f;

	float fov;
	float x;
	float y;

	glm::vec3 position;
	glm::vec3 target; //front.
	glm::vec3 up;
};

enum class CameraType
{
	Perspective,
	Orthographic
};

std::string getCameraTypeString(CameraType type);

class Camera
{
	public:
		Camera();
		Camera(const std::string& name, const Frustrum& frustrum);
		Camera(const std::string& name, glm::vec3 position, float x, float y);
		~Camera();

		void recalculate();

		void setCamera(CameraType cameraType);
		void setName(const std::string& name);

		void setAspectRatio(float x, float y);
		/// @brief setters override existing values
		///        updaters add onto existin values
		void setPosition(glm::vec3 position);
		void setTarget(glm::vec3 target);

		void setNearPlane(float nearPlane);
		void setFarPlane(float farPlane);

		void setFOV(float FOV);

		void setType(CameraType type);

		void updatePosition(glm::vec3 position);
		void updateTarget(glm::vec3 target);

		glm::vec3 getRightVector()const;
		glm::vec3 getViewDirection()const;

		glm::mat4 getViewProject()const;
		glm::mat4 getView();
		glm::mat4 getProjection();

		const Frustrum& getFrustrum()const;
		Frustrum& getFrustrum();

		const CameraType& getCameraType()const;
		const std::string& getName()const;
		std::string& getName();

		bool operator==(const Camera& other) const
		{
			return name == other.name;
		}

	private:
		std::string name;
		
		Frustrum frustrum;
		CameraType type;

		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewProjection;
};

