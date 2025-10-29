#include "ResourceCache.h"

namespace Iaonnis
{
	std::shared_ptr<Material> defaultMaterial = nullptr;

	std::shared_ptr<ImageTexture> flatDiffuse = nullptr;
	std::shared_ptr<ImageTexture> flatNormal = nullptr;
	std::shared_ptr<ImageTexture> flatAO = nullptr;
	std::shared_ptr<ImageTexture> flatRoughness = nullptr;
	std::shared_ptr<ImageTexture> flatMetallic = nullptr;

	std::unordered_map<IconType, std::shared_ptr<ImageTexture>>defaultIcons;



	std::shared_ptr<Material> ResourceCache::GetDefaultMaterial()
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

	std::shared_ptr<ImageTexture> Iaonnis::ResourceCache::GetDefaultByTextureType(TextureMapType type)
	{
		switch (type)
		{
		case TextureMapType::Albedo: return flatDiffuse;
		case TextureMapType::Normal:return flatNormal;
		case TextureMapType::AO:return flatAO;
		case TextureMapType::Roughness:return flatRoughness;
		case TextureMapType::Metallic:return flatMetallic;
		}

		return nullptr;
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
		defaultIcons[IconType::New] = load<ImageTexture>("Assets/Icons/File.png");
		defaultIcons[IconType::Duplicate] = load<ImageTexture>("Assets/Icons/duplicate.png");
		defaultIcons[IconType::Remove] = load<ImageTexture>("Assets/Icons/x.png");
		defaultIcons[IconType::Open] = load<ImageTexture>("Assets/Icons/Folder Line.png");
		defaultIcons[IconType::Save] = load<ImageTexture>("Assets/Icons/Save.png");
		defaultIcons[IconType::Resources] = load<ImageTexture>("Assets/Icons/Resource.png");
		defaultIcons[IconType::SolidMode] = load<ImageTexture>("Assets/Icons/Solid Mode.png");
		defaultIcons[IconType::RenderMode] = load<ImageTexture>("Assets/Icons/Render Mode.png");
		defaultIcons[IconType::RenderOption] = load<ImageTexture>("Assets/Icons/Render Options.png");
		defaultIcons[IconType::Capture] = load<ImageTexture>("Assets/Icons/Capture.png");
		defaultIcons[IconType::Entities] = load<ImageTexture>("Assets/Icons/Entities.png");
		defaultIcons[IconType::ListAdd] = load<ImageTexture>("Assets/Icons/List Add.png");
		defaultIcons[IconType::ListLayout] = load<ImageTexture>("Assets/Icons/List Layout.png");
		defaultIcons[IconType::GridLayout] = load<ImageTexture>("Assets/Icons/Grid Layout.png");
		defaultIcons[IconType::Settings] = load<ImageTexture>("Assets/Icons/Settings.png");
		defaultIcons[IconType::Sync] = load<ImageTexture>("Assets/Icons/sync.png");
		defaultIcons[IconType::Edit] = load<ImageTexture>("Assets/Icons/Edit.png");
		defaultIcons[IconType::Info] = load<ImageTexture>("Assets/Icons/info.png");

		defaultIcons[IconType::Unknown] = load<ImageTexture>("Assets/Icons/Unknown.png");
		return IAONNIS_LOG_DEBUG("Default Icons Loaded.");
	}

	ResourceCache::ResourceCache()
	{
		stbi_set_flip_vertically_on_load(true);

		memset(&meta, 0, sizeof(ResourceCacheMeta));

		auto cube = create<Mesh>("Cube.mesh");
		auto plane = create<Mesh>("Plane.mesh");

		Mesh::generateCube(cube.get());
		Mesh::generatePlane(plane.get());

		flatDiffuse = load<ImageTexture>("Assets/Textures/default_diffuse.png");
		flatNormal  = load<ImageTexture>("Assets/Textures/default_normal.png");
		flatAO      = load<ImageTexture>("Assets/Textures/default_ambient_occlusion.png");
		flatRoughness = load<ImageTexture>("Assets/Textures/default_roughness.png");
		flatMetallic = load<ImageTexture>("Assets/Textures/default_metallic.png");

		defaultMaterial = create<Material>("Material.yaml");
		defaultMaterial->setDiffuseMap(flatDiffuse->GetID());
		defaultMaterial->setNormalMap(flatNormal->GetID());
		defaultMaterial->setAoMap(flatAO->GetID());
		defaultMaterial->setRoughnessMap(flatRoughness->GetID());
		defaultMaterial->setMetallicMap(flatMetallic->GetID());

		defaultMaterial->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		LoadDefaultIcons();

	}

	ResourceCache::~ResourceCache()
	{

	}

	std::shared_ptr<Material> ResourceCache::CreateNewMaterial(const std::string& name)
	{

		filespace::filepath resourcePath = name + ".yaml";
		resourcePath = GenerateDuplicateResourceName<Material>(resourcePath);

		std::shared_ptr<Material> newResource = create<Material>(resourcePath);

		newResource->setDiffuseMap(flatDiffuse->GetID());
		newResource->setNormalMap(flatNormal->GetID());
		newResource->setAoMap(flatAO->GetID());
		newResource->setRoughnessMap(flatRoughness->GetID());
		newResource->setMetallicMap(flatMetallic->GetID());

		newResource->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		return newResource;
	}

}
