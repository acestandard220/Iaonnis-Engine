#pragma once
#include "../Core/Core.h"
#include "../Core/pch.h"

#include "Camera.h"

namespace Iaonnis
{
	struct DerivedComponent
	{
		bool active = true;
	};

	struct IDComponent : public DerivedComponent
	{
		UUID id;

		IDComponent()
		{
			id = UUIDFactory::generateUUID();
		}

		IDComponent(const IDComponent& other) = default;

		operator UUID() { return id; }
	};

	struct TagComponent : public DerivedComponent
	{
		std::string tag;

		TagComponent()
		{
			
		};
		TagComponent(std::string name)
		{
			tag = name;
		}
		TagComponent(const TagComponent& other) = default;
	};

	struct TransformComponent : public DerivedComponent
	{
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		glm::mat4 model = glm::mat4(1.0f);;

		TransformComponent()
			:position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(1.0, 1.0f, 1.0f) {
		}
		TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
			:position(position), rotation(rotation), scale(scale) {
		}
		TransformComponent(const TransformComponent& other) = default;
	};

	struct MeshFilterComponent : public DerivedComponent
	{
		UUID meshID;

		//std::vector<UUID> materialID;
		std::unordered_map<UUID, std::list<int>> materialIDMap;
		std::vector<std::string> names;

		MeshFilterComponent() = default;
		MeshFilterComponent(UUID id)
			:meshID(id){ }
		MeshFilterComponent(const MeshFilterComponent& other) = default;
	};

	struct CameraComponent : public DerivedComponent
	{
		std::shared_ptr<Camera> camera;
		bool primary = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;
	};

	enum class LightType
	{
		Directional,
		Point,
		Spot
	};

	struct LightComponent : public DerivedComponent
	{
		LightType type;
		glm::vec4 color;

		glm::vec3 position;


		//Spot Light Params
		float innerRadius;
		float outerRadius;
		glm::vec3 spotDirection;
		
		LightComponent()
			:type(LightType::Point), color(1.0f, 1.0f, 1.0f, 1.0f), innerRadius(5.0f), outerRadius(30.0f),spotDirection(0.0f,-1.0f,0.0f)
		{}
		LightComponent(const LightComponent& other) = default;
	};
}
