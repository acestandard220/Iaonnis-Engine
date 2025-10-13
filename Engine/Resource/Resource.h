#pragma once
#include "../Core/Core.h"
#include "../Core/pch.h"

namespace Iaonnis
{
	enum class ResourceType
	{
		Mesh,
		Material,
		ImageTexture,

		Unknown
	};

	class ResourceCache;
	class Resource
	{
	public: 
		Resource() = default;
		~Resource() = default;

		virtual void load(filespace::filepath path) = 0;
		virtual void save(filespace::filepath path) = 0;

		UUID& GetID(){ return id; }
		const UUID& GetID()const;
		const std::string& getName()const;
		const filespace::filepath& getPath()const;
		ResourceType getType()const;

		static std::string getTypeString(ResourceType type);
		
	protected:
		friend 	class ResourceCache;

		void setUUID(UUID i) { id = i; }
		void setName(const std::string& nme) { name = nme; }
		void setPath(filespace::filepath p) { path = p; }

		void use(int count = 1) { refCount += count; }
		void unuse(int count = 1);

	protected:
		UUID id;
		std::string name;
		filespace::filepath path;
		ResourceType type;

		int refCount = 0;
	};
}
