#pragma once
#include "../Core/Core.h"
#include "../Core/pch.h"
#include "Components.h"
#include "Scene.h"


namespace Iaonnis
{
	class Scene;
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entity, Scene* scene)
			:entity(entity), scene(scene), active(true)
		{
		}
		Entity(const Entity& other) = default;

		entt::entity GetBaseEntity()
		{
			return entity;
		}
		glm::mat4 GetTransformMatrix()
		{
			return GetComponent<TransformComponent>().model;
		}

		UUID GetUUID()
		{
			return scene->registry.get<IDComponent>(entity).id;
		}

		std::string GetTag()
		{
			return GetComponent<TagComponent>().tag;
		}

		template<typename T>
		bool HasComponent()
		{
			return scene->registry.any_of<T>(entity);
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return scene->registry.emplace<T>(entity, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			return scene->registry.get<T>(entity);
		}

		template<typename T> 
		void RemoveComponent()
		{
			return scene->registry.remove<T>(entity);
		}

		bool* GetActive() { return &active; }

		bool active;
	protected:
		Entity* parent = nullptr;
		std::vector<Entity*> children;
		
	private:
		entt::entity entity;
		Scene* scene;
	};
}