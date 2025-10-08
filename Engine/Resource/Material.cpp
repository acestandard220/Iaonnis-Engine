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
	UUID Material::getDiffuseID() const
	{
		return albedo.diffuseMap;
	}
	UUID Material::getNormalID() const
	{
		return normal.normalMap;
	}
	float Material::getNormalStrength() const
	{
		return normal.normalStrength;
	}
}