#pragma once
#include "../Resource/ResourceCache.h"
#include "Camera.h"
#include "Systems.h"

namespace Iaonnis
{
	class Entity;
	class Scene
	{
		public:
			Scene(const std::string& name);
			~Scene();

			void save(filespace::filepath path);

			void OnUpdate(float dt);

			Entity& createEntity(const std::string& name);
			Entity& CreateCamera(const std::string& name);

			Entity& addMesh(filespace::filepath path, const std::string& name);
			Entity& addMesh(UUID meshID);

			Entity& addDirectionalLight(glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f));
			Entity& addSpotLight();
			Entity& addPointLight();

			Entity& addCamera();

			Entity& addCube(const std::string& name);
			Entity& addPlane(const std::string& name);


			void removeEntity(Entity entity);

			template<typename... T>
			std::vector<Entity> getEntitiesWith() {
				std::vector<Entity> ents;
				auto view = registry.view<T...>();

				for (auto entity : view) {
					ents.emplace_back(entity, this); 
				}

				return ents;
			}
			
			std::vector<Entity>& GetEntities()
			{
				return entities;
			}

			Entity& GetEntity(UUID id);

			std::shared_ptr<ResourceCache> getCache() { return cache; }
			const std::string& getName()const { return name; }
			
			void OnEntityRegisteryModified() { isEntityRegDirty = true; }
			void setEntityRegisteryClean() { isEntityRegDirty = false; }

			bool isEntityRegisteryDirty()const { return isEntityRegDirty; }

			std::shared_ptr<Camera> GetSceneCamera() { return camera; }

			std::shared_ptr<Environment> GetEnvironment() { return environment; }

		private:
			void OnViewFrameResize(Event& event);
		private:
			friend class Entity;
			entt::registry registry;

			std::vector<Entity> entities;
			std::shared_ptr<ResourceCache> cache;

			std::string name;
			bool isEntityRegDirty = true;

			glm::vec2 displaySize;
			std::shared_ptr<Camera> camera;

			std::shared_ptr<Environment> environment;

			std::vector<std::unique_ptr<System>> systems;
	};
}