#pragma once
#include "ImageTexture.h"

namespace Iaonnis
{
	enum class TextureMapType
	{
		Albedo,Normal,AO,Roughness,Metallic
	};

	struct AlbedoProperty
	{
		UUID diffuseMap;
		glm::vec4 color;

		AlbedoProperty() = default;
		AlbedoProperty(const AlbedoProperty& other)
			:diffuseMap(other.diffuseMap),color(other.color)
		{

		}
	};

	struct NormalProperty
	{
		UUID normalMap;
		float normalStrength;
		float flipY;

		NormalProperty() = default;
		NormalProperty(const NormalProperty& other)
			:normalMap(other.normalMap),
			normalStrength(other.normalStrength),
			flipY(other.flipY)
		{
		}

	};

	class Material :public Resource
	{
	public:
		Material();
		Material(const Material& other);

		~Material();

		void load(filespace::filepath path) override;
		void save(filespace::filepath path) override;

		//Blind Functions
		void SetMap(TextureMapType type, UUID map);
		UUID& GetMap(TextureMapType type);
		static std::string GetMapTypeString(TextureMapType type);

		void setColor(glm::vec4 color);
		void setDiffuseMap(UUID diffuseID);

		void setNormalMap(UUID normalID);
		void setNormalStrenght(float strength);
		void flipNormalY();

		void setAoMap(UUID aoMap);
		void setRoughnessMap(UUID roughnessMap);
		void setMetallicMap(UUID metallicMap);

		void setUVScale(glm::vec2 scale);

		glm::vec2 getUVScale()const;
		glm::vec4 getColor()const;

		glm::vec2& GetUVScale() { return uvScale; }
		glm::vec4& GetColor() { return albedo.color; }
		float& GetNormalStrength() { return normal.normalStrength; }

		const UUID getDiffuseID()const;
		const UUID getNormalID()const;
		const UUID getAoID()const;
		const UUID getRoughnessID()const;
		const UUID getMetallicID()const;

		UUID& GetDiffuseID() { return albedo.diffuseMap; }
		UUID& GetNormalID() { return normal.normalMap; }
		UUID& GetAoID() { return aoMap; }
		UUID& GetRoughnessID() { return roughnessMap; }
		UUID& getMetallicID();

		float getNormalStrength()const;

	private:
		AlbedoProperty albedo;
		NormalProperty normal;

		UUID aoMap;
		UUID roughnessMap;
		UUID metallicMap;

		glm::vec2 uvScale;
	};
}