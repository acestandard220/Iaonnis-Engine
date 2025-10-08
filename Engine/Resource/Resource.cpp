#include "Resource.h"

namespace Iaonnis
{
	const UUID& Resource::getID() const
	{
		return id;
	}

	const std::string& Resource::getName()const
	{
		return name;
	}

	const filespace::filepath& Resource::getPath() const
	{
		return path;
	}

	ResourceType Resource::getType() const
	{
		return type;
	}

	std::string Resource::getTypeString(ResourceType type)
	{
		switch (type)
		{
			case ResourceType::Mesh: return "Mesh";
			case ResourceType::Material: return "Material";
			case ResourceType::ImageTexture: return "ImageTexture";
		}

		return "Unknown";
	}

}