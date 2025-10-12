#pragma once
#include "../Core/Core.h"
#include "../Core/pch.h"

#include "Resource.h"

namespace Iaonnis
{
	struct SubMeshTexturePaths
	{
		filespace::filepath diffuseMap;
		filespace::filepath normalMap;
		filespace::filepath aoMap;
		filespace::filepath roughnessMap;
		filespace::filepath metallicMap;
	};

	struct Vertice
	{
		glm::vec3 p;
		glm::vec3 n;
		glm::vec2 uv;

		glm::vec3 tangent;
		glm::vec3 bitangent;
	};

	struct SubMesh
	{
		uint32_t vertexOffset;
		uint32_t vertexCount;

		uint32_t indexOffset;
		uint32_t indexCount;

		int index;
		std::string name;
	};

	class Mesh : public Resource
	{
		public:
			Mesh();
			Mesh(const Mesh& other);
			~Mesh() = default;

			virtual void load(filespace::filepath path)override;
			virtual void save(filespace::filepath path)override;

			SubMesh* getSubMesh(int index);
			int getSubMeshCount()const { return subMeshes.size(); }

			Vertice* getSubMeshVerticeStart(int index) { return &vertices[subMeshes[index].vertexOffset]; }
			uint32_t* getSubMeshIndexStart(int index) { return &indices[subMeshes[index].vertexOffset]; }

			const std::vector<Vertice>& getVertices()const;
			const std::vector<uint32_t>& getIndices()const;

			SubMeshTexturePaths& GetFileTexturePaths(int index);

			static void generateCube(Mesh* mesh);
			static void generatePlane(Mesh* mesh);
			static void generateCylinder(Mesh* mesh);
			static void generateCone(Mesh* mesh);
		
		private:
			void loadObjFile(filespace::filepath path);
			void loadMeshFile(filespace::filepath path);

			void saveMeshFile(filespace::filepath path);

			void generateTangentBitangent();
			void generateNormals();
		private:
			std::vector<Vertice> vertices;
			std::vector<uint32_t> indices;
			std::vector<SubMesh> subMeshes;

			std::vector<SubMeshTexturePaths> texturePaths;
	};

}