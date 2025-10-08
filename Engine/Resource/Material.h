#pragma once
#include "ImageTexture.h"

namespace Iaonnis
{
	struct AlbedoProperty
	{
		UUID diffuseMap;
		glm::vec4 color;
	};

	struct NormalProperty
	{
		UUID normalMap;
		float normalStrength;
		float flipY;
	};

	class Material :public Resource
	{
	public:
		Material();
		~Material();

		void load(filespace::filepath path) override;
		void save(filespace::filepath path) override;

		void setColor(glm::vec4 color);
		void setDiffuseMap(UUID diffuseID);

		void setNormalMap(UUID normalID);
		void setNormalStrenght(float strength);
		void flipNormalY();

		void setUVScale(glm::vec2 scale);

		glm::vec2 getUVScale()const;
		glm::vec4 getColor()const;
		UUID getDiffuseID()const;
		UUID getNormalID()const;
		float getNormalStrength()const;

	private:
		AlbedoProperty albedo;
		NormalProperty normal;

		glm::vec2 uvScale;
	};
}