#include "ResourceCache.h"

namespace Iaonnis
{
	std::shared_ptr<Material> defaultMaterial = nullptr;
	std::shared_ptr<Material> rockMaterial = nullptr;

	std::shared_ptr<ImageTexture> rockDiffuse = nullptr;
	std::shared_ptr<ImageTexture> flatDiffuse = nullptr;
	std::shared_ptr<ImageTexture> flatNormal = nullptr;

	std::unordered_map<IconType, std::shared_ptr<ImageTexture>>defaultIcons;

	std::shared_ptr<Material> ResourceCache::getDefaultMaterial()
	{
		return defaultMaterial;
	}

	std::shared_ptr<ImageTexture> ResourceCache::GetDefaultDiffuse()
	{
		return flatDiffuse;
	}

	std::shared_ptr<ImageTexture> ResourceCache::GetDefaultNormal()
	{
		return flatNormal;
	}

	std::shared_ptr<Material> ResourceCache::GetRockMaterial()
	{
		return rockMaterial;
	}

	std::shared_ptr<ImageTexture> ResourceCache::GetIcon(IconType iconType)
	{
		if (defaultIcons.find(iconType) == defaultIcons.end())
		{
			IAONNIS_LOG_ERROR("Failed to find IconType.");
			return nullptr;
		}

		return defaultIcons[iconType];
	}

	void ResourceCache::LoadDefaultIcons()
	{
		defaultIcons[IconType::Plus] = load<ImageTexture>("Assets/Icons/plus.png");

		return IAONNIS_LOG_DEBUG("Default Icons Loaded.");
	}

	ResourceCache::ResourceCache()
	{
		auto cube = create<Mesh>("Cube.mesh");
		auto plane = create<Mesh>("Plane.mesh");

		Mesh::generateCube(cube.get());
		Mesh::generatePlane(plane.get());

		flatDiffuse = load<ImageTexture>("Assets/Textures/default_diffuse.png");
		flatNormal  = load<ImageTexture>("Assets/Textures/default_normal.png");
		rockDiffuse = load<ImageTexture>("Assets/Textures/Image.jpg");

		defaultMaterial = create<Material>("Material.yaml");
		defaultMaterial->setDiffuseMap(flatDiffuse->getID());
		defaultMaterial->setNormalMap(flatNormal->getID());
		defaultMaterial->setColor(glm::vec4(0.20f, 0.5f, 0.6f, 1.0f));

		rockMaterial = create<Material>("Rock Material.yaml");
		rockMaterial->setDiffuseMap(rockDiffuse->getID());
		rockMaterial->setNormalMap(flatNormal->getID());
		rockMaterial->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		

		LoadDefaultIcons();

	}

	ResourceCache::~ResourceCache()
	{

	}


}
