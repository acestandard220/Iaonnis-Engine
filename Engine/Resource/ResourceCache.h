#pragma once
#include "../Core/Core.h"
#include "../Core/pch.h"

#include "Resource.h"
#include "Mesh.h"
#include "ImageTexture.h"
#include "Material.h"
#include "Environment.h"

namespace Iaonnis
{
	enum class IconType
	{
		Folder,
		File,
		Plus
	};

	class ResourceCache
	{
	public:
		ResourceCache();
		~ResourceCache();

		template<class T>
		std::shared_ptr<T> getByPath(filespace::filepath path)
		{
			for (auto& [id, resource] : resources)
			{
				if (resource->getPath() == path)
				{
					return std::static_pointer_cast<T>(resource);
				}
			}

			IAONNIS_LOG_ERROR("Failed to find resource. (Path = %s)", path.string().c_str());
			return nullptr;
		}

		//Avoid getting by name. Only use when you can are sure the asset/resource is the only one with that name.
		template<class T>
		std::shared_ptr<T> getByName(const std::string& name)
		{
			for (auto& [id, resource] : resources)
			{
				if (resource->getName() == name)
					return std::static_pointer_cast<T>(resource);
			}

			IAONNIS_LOG_ERROR("Failed to find resource. (Name = %s)", name.c_str());
			return nullptr;
		}

		template<class T>
		std::vector<std::shared_ptr<T>> getByType(ResourceType type)
		{
			std::vector<std::shared_ptr<T>> res;
			for (auto& [id, resource] : resources)
			{
				if (resource->getType() == type)
					res.push_back(std::static_pointer_cast<T>(resource));
			}

			return res;
		}

		template<class T>
		std::shared_ptr<T> getByUUID(UUID id)
		{
			if (resources.find(id) != resources.end())
			{
				return std::static_pointer_cast<T>(resources[id]);
			}

			IAONNIS_LOG_ERROR("Failed to find resource. (UUID = %s)", UUIDFactory::uuidToString(id).c_str());
			return nullptr;
		}

		template<class T> 
		std::shared_ptr<T> create(filespace::filepath path)
		{
			std::shared_ptr<T> newResource = std::make_shared<T>();
			cache<T>(path, newResource);
			return newResource;
		}

		template<class T>
		std::shared_ptr<T> load(filespace::filepath path)
		{
			if (!filespace::exists(path))
			{
				IAONNIS_LOG_ERROR("Invalid path provided. (Path = %s)", path.string().c_str());
				return nullptr;
			}

			std::shared_ptr<T> existing = getByPath<T>(path);
			if (existing)
			{
				IAONNIS_LOG_ERROR("Resource has already been cached. (Path = %s)", path.string().c_str());
				return std::static_pointer_cast<T>(existing);
			}
			
			std::shared_ptr<T> newResource = std::make_shared<T>();
			newResource->load(path);
			cache(path, newResource);
			return newResource;
		}

		template<class T>
		void save(const std::string& name, filespace::filepath path)
		{
			std::shared_ptr<T> existing = getByPath<T>(path);
			if (existing)
			{
				IAONNIS_LOG_ERROR("Resource has already been cached. (Path = %s)", path.string().c_str());
				return std::static_pointer_cast<T>(existing);
			}

			existing->setPath(path);
			existing->setName(filespace::getStem(path));
			
			existing->save(path);
		}



		template<class T>
		void use(UUID id)
		{
			auto resource = getByUUID<T>(id);
			if (resource == nullptr)
			{
				IAONNIS_LOG_ERROR("Failed to find resource. (UUID = %s)", UUIDFactory::uuidToString(id));
				return;
			}

			resource->use();
		}

		template<class T>
		void unsee(UUID id)
		{
			auto resource = getByUUID<T>(id);
			if (resource == nullptr)
			{
				IAONNIS_LOG_ERROR("Failed to find resource. (Name = %s)", UUIDFactory::uuidToString(id));
				return;
			}

			resource->unuse();
		}

		std::shared_ptr<Material> getDefaultMaterial();
		std::shared_ptr<ImageTexture> GetDefaultDiffuse();
		std::shared_ptr<ImageTexture> GetDefaultNormal();
		std::shared_ptr<Material> GetRockMaterial();

		static std::shared_ptr<ImageTexture> GetIcon(IconType iconType);


	private:
			void LoadDefaultIcons();
			
			template<class T>
			void cache(filespace::filepath path, std::shared_ptr<T> resource)
			{
				UUID id = UUIDFactory::generateUUID();
				resource->setUUID(id);
				resource->setPath(path);
				resource->setName(filespace::getStem(path));

				resources[id] = resource;
			}

			template<class T>
			void cache(std::shared_ptr<T> resource)
			{
				UUID id = UUIDFactory::generateUUID();
				resource->setUUID(id);

				resources[id] = resource;
			}
	private:
		std::unordered_map <UUID, std::shared_ptr<Resource>>resources;
	};

}