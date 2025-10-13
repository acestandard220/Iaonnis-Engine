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

		UUID GetSubMeshMaterial(int index)
		{

			if (!HasComponent<MeshFilterComponent>())
			{
				IAONNIS_LOG_ERROR("Cannot add material to entity without a mesh filter");
				return UUIDFactory::getInvalidUUID();
			}

			auto& meshFilter = GetComponent<MeshFilterComponent>();
			auto& materialMap = meshFilter.materialIDMap;

			for (auto& [mtl, deps] : materialMap)
			{
				for (auto dep : deps)
				{
					if (dep == index)
						return mtl;
				}
			}

			return UUIDFactory::getInvalidUUID();
		}

		void DettachUnusedMaterials()
		{
			if (!HasComponent<MeshFilterComponent>())
			{
				IAONNIS_LOG_ERROR("Cannot add material to entity without a mesh filter");
				return;
			}

			auto& meshFilter = GetComponent<MeshFilterComponent>();
			auto& materialMap = meshFilter.materialIDMap;

			std::set<UUID>toClear;
			for (auto& [matID, deps] : materialMap)
			{
				if(!deps.size())
				{
					toClear.insert(matID);
				}
			}

			for(auto& c : toClear)
			{
				materialMap.erase(c);
			}
		}

		void AssignMaterial(UUID mtlID, int subMeshIndex)
		{
			if (!HasComponent<MeshFilterComponent>())
			{
				IAONNIS_LOG_ERROR("Cannot add material to entity without a mesh filter");
				return;
			}

			auto& meshFilter = GetComponent<MeshFilterComponent>();
			auto& materialMap = meshFilter.materialIDMap;

			for (auto& [matID, dependants] : materialMap)
			{
				dependants.remove(subMeshIndex);
			}

			meshFilter.materialIDMap[mtlID].push_back(subMeshIndex);
			return;
		}

		void AssignGlobalMaterial(UUID mtlID)
		{
			if (!HasComponent<MeshFilterComponent>())
			{
				IAONNIS_LOG_ERROR("Cannot add material to entity without a mesh filter");
				return;
			}
			auto& meshFilter = GetComponent<MeshFilterComponent>();
			for (int subMeshIndex = 0; subMeshIndex < meshFilter.names.size(); subMeshIndex++)
			{
				AssignMaterial(mtlID, subMeshIndex);
			}
			return;
		}

		void ResetMaterial(int subMeshIndex)
		{
			return AssignMaterial(ResourceCache::getDefaultMaterial()->GetID(), subMeshIndex);
		}

		void ResetAllSubMeshMaterials()
		{
			if (!HasComponent<MeshFilterComponent>())
			{
				IAONNIS_LOG_ERROR("Cannot add material to entity without a mesh filter");
				return;
			}
			auto& meshFilter = GetComponent<MeshFilterComponent>();
			for (int subMeshIndex = 0; subMeshIndex < meshFilter.names.size(); subMeshIndex++)
			{
				ResetMaterial(subMeshIndex);
			}
			return;
		}

		int GetSubMeshCount()
		{
			if (!HasComponent<MeshFilterComponent>())
			{
				IAONNIS_LOG_ERROR("Entity does not have mesh filter.");
				return -1;
			}
			auto& meshFilter = GetComponent<MeshFilterComponent>();
			return meshFilter.names.size();
		}

		std::vector<UUID> GetMaterialsInUse()
		{
			std::vector<UUID> ret;
			if (!HasComponent<MeshFilterComponent>())
			{
				IAONNIS_LOG_ERROR("Entity does not have mesh filter.");
				return ret;
			}

			auto& meshFilter = GetComponent<MeshFilterComponent>();
			for (auto id : meshFilter.materialIDMap)
			{
				ret.push_back(id.first);
			}

			return ret;
		}

		int GetMaterialDependantCount(UUID mtl)
		{
			if (!HasComponent<MeshFilterComponent>())
			{
				IAONNIS_LOG_ERROR("Entity does not have mesh filter.");
				return -1;
			}
			auto& meshFilter = GetComponent<MeshFilterComponent>();
			
			if (meshFilter.materialIDMap.find(mtl) == meshFilter.materialIDMap.end())
			{
				return 0;
			}

			return meshFilter.materialIDMap[mtl].size();
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