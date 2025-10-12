#include "Material.h"

namespace Iaonnis {
	Material::Material()
	{
		type = ResourceType::Material;

		uvScale = glm::vec2(1.0f, 1.0f);
		albedo.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		albedo.diffuseMap = UUIDFactory::getInvalidUUID();
		normal.normalMap  = UUIDFactory::getInvalidUUID();
	}

	Material::Material(const Material& other)
		: uvScale(other.uvScale)
	{
		albedo = other.albedo;
		normal = other.normal;
		aoMap = other.aoMap;
		roughnessMap = other.roughnessMap;
		metallicMap = other.metallicMap;
		refCount = 0;
	}

	Material::~Material()
	{
	}

	void Material::load(filespace::filepath path)
	{
		IAONNIS_LOG_WARN("Not Loading Material Resource");
	}

	void Material::save(filespace::filepath path)
	{
		IAONNIS_LOG_WARN("Not Saving Material Resource");
	}

	void Iaonnis::Material::SetMap(TextureMapType type, UUID map)
	{
		switch (type)
		{
			case TextureMapType::Albedo: return setDiffuseMap(map);
			case TextureMapType::Normal: return setNormalMap(map);
			case TextureMapType::AO:     return setAoMap(map);
			case TextureMapType::Roughness:return setRoughnessMap(map);
			case TextureMapType::Metallic: return setMetallicMap(map);
		}
	}

	UUID& Material::GetMap(TextureMapType type)
	{
		switch (type)
		{
		case TextureMapType::Albedo: return GetDiffuseID();
		case TextureMapType::Normal: return GetNormalID();
		case TextureMapType::AO:     return GetAoID();
		case TextureMapType::Roughness:return GetRoughnessID();
		case TextureMapType::Metallic: return getMetallicID();
		}
	}

	std::string Material::GetMapTypeString(TextureMapType type)
	{
		switch (type)
		{
			case TextureMapType::Albedo: return "Diffuse";
			case TextureMapType::Normal: return "Normal";
			case TextureMapType::AO:     return "Ambient Occlusion";
			case TextureMapType::Roughness:return "Roughness";
			case TextureMapType::Metallic: return "Metallic";
		}

		return "Unknown";
	}

	void Material::setColor(glm::vec4 color)
	{
		albedo.color = color;
	}
	void Material::setDiffuseMap(UUID diffuseID)
	{
		albedo.diffuseMap = diffuseID;
	}
	void Material::setNormalMap(UUID normalID)
	{
		normal.normalMap = normalID;
	}
	void Material::setNormalStrenght(float strength)
	{
		normal.normalStrength = strength;
	}
	void Material::flipNormalY()
	{
		normal.flipY *= -1;
	}
	void Material::setAoMap(UUID map)
	{
		aoMap = map;
	}
	void Iaonnis::Material::setRoughnessMap(UUID rMap)
	{
		roughnessMap = rMap;
	}
	void Material::setMetallicMap(UUID map)
	{
		metallicMap = map;
	}
	void Material::setUVScale(glm::vec2 scale)
	{
		uvScale = scale;
	}
	glm::vec2 Material::getUVScale() const
	{
		return uvScale;
	}
	glm::vec4 Material::getColor() const
	{
		return albedo.color;
	}
	const UUID Material::getDiffuseID() const
	{
		return albedo.diffuseMap;
	}
	const UUID Material::getNormalID() const
	{
		return normal.normalMap;
	}
	const UUID Material::getAoID() const
	{
		return aoMap;
	}
	const UUID Material::getRoughnessID() const
	{
		return roughnessMap;
	}
	const UUID Material::getMetallicID() const
	{
		return metallicMap;
	}
	UUID& Material::getMetallicID()
	{
		return metallicMap;
	}
	float Material::getNormalStrength() const
	{
		return normal.normalStrength;
	}
}