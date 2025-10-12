#include "Mesh.h"

namespace Iaonnis
{
    struct MeshFileHeader
    {
        char magic[2]{ 'D','2' };
        uint32_t version = 1;

        size_t meshCount;
        size_t vertexCount;
        size_t indexCount;

        size_t stringTableOffset;
        size_t stringTableLength;
    };

    struct SubMeshHeader
    {
        uint32_t vertexOffset;
        uint32_t vertexCount;
        uint32_t indexOffset;
        uint32_t indexCount;

        uint32_t nameOffset;
        uint32_t nameLength;
    };


	Mesh::Mesh()
	{
		type = ResourceType::Mesh;
	}

    Mesh::Mesh(const Mesh& other)
        :vertices(other.vertices), indices(other.indices), subMeshes(other.subMeshes)
    {
        refCount = 0;
    }

	void Mesh::load(filespace::filepath path)
	{
        const std::string extension = path.extension().string();

        if (extension == ".obj")
        {
            loadObjFile(path);
        }
        else if (extension == ".mesh")
        {
            loadMeshFile(path);
        }
	}

	void Mesh::save(filespace::filepath path)
	{
        std::string extension = path.extension().string();

        if (extension == ".mesh")
        {
            saveMeshFile(path);
        }
        else if (extension == ".obj")
        {
            IAONNIS_LOG_WARN("Avoid saving meshes in obj format they are slow.\nGLTF/GLB or Mesh File is recommended.");
        }

	}

	SubMesh* Mesh::getSubMesh(int index) 
	{
		if (index > subMeshes.size())
			return nullptr;
		
		return &subMeshes[index];
	}

    const std::vector<Vertice>& Mesh::getVertices() const
    {
        return vertices;
    }

    const std::vector<uint32_t>& Mesh::getIndices() const
    {
        return indices;
    }

    SubMeshTexturePaths& Mesh::GetFileTexturePaths(int index)
    {
        if (index > texturePaths.size())
        {
            IAONNIS_LOG_ERROR("Invalid submesh index");
            return texturePaths[0];
        }

        return texturePaths[index];
    }

    void Mesh::generateCube(Mesh* mesh)
    {
        SubMesh subMesh;
        subMesh.indexCount = 36;
        subMesh.indexOffset = 0;
        subMesh.vertexCount = 24;
        subMesh.vertexOffset = 0;
        subMesh.name = mesh->name;
        subMesh.index = 0;

        mesh->subMeshes.push_back(subMesh);

        //Counter ClockWise
       //Front (facing -Z)
        mesh->vertices.push_back({ {-1.0f,-1.0f,-1.0f}, { 0.0f, 0.0f,-1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ { 1.0f,-1.0f,-1.0f}, { 0.0f, 0.0f,-1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ { 1.0f, 1.0f,-1.0f}, { 0.0f, 0.0f,-1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ {-1.0f, 1.0f,-1.0f}, { 0.0f, 0.0f,-1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        //Back (facing +Z)
        mesh->vertices.push_back({ { 1.0f,-1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ {-1.0f,-1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ {-1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ { 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        //Left (facing -X)
        mesh->vertices.push_back({ {-1.0f,-1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ {-1.0f,-1.0f,-1.0f}, {-1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ {-1.0f, 1.0f,-1.0f}, {-1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ {-1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        //Right (facing +X)
        mesh->vertices.push_back({ { 1.0f,-1.0f,-1.0f}, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ { 1.0f,-1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ { 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ { 1.0f, 1.0f,-1.0f}, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        //Top (facing +Y)
        mesh->vertices.push_back({ {-1.0f, 1.0f,-1.0f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ { 1.0f, 1.0f,-1.0f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ { 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ {-1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        //Bottom (facing -Y)
        mesh->vertices.push_back({ {-1.0f,-1.0f, 1.0f}, { 0.0f,-1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ { 1.0f,-1.0f, 1.0f}, { 0.0f,-1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ { 1.0f,-1.0f,-1.0f}, { 0.0f,-1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        mesh->vertices.push_back({ {-1.0f,-1.0f,-1.0f}, { 0.0f,-1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        //Front
        mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(2);
        mesh->indices.push_back(2); mesh->indices.push_back(3); mesh->indices.push_back(0);

        //Back
        mesh->indices.push_back(4); mesh->indices.push_back(5); mesh->indices.push_back(6);
        mesh->indices.push_back(6); mesh->indices.push_back(7); mesh->indices.push_back(4);

        //Left
        mesh->indices.push_back(8);  mesh->indices.push_back(9);  mesh->indices.push_back(10);
        mesh->indices.push_back(10); mesh->indices.push_back(11); mesh->indices.push_back(8);

        //Right
        mesh->indices.push_back(12); mesh->indices.push_back(13); mesh->indices.push_back(14);
        mesh->indices.push_back(14); mesh->indices.push_back(15); mesh->indices.push_back(12);

        //Top
        mesh->indices.push_back(16); mesh->indices.push_back(17); mesh->indices.push_back(18);
        mesh->indices.push_back(18); mesh->indices.push_back(19); mesh->indices.push_back(16);

        //Bottom
        mesh->indices.push_back(20); mesh->indices.push_back(21); mesh->indices.push_back(22);
        mesh->indices.push_back(22); mesh->indices.push_back(23); mesh->indices.push_back(20);

        mesh->generateTangentBitangent();
        mesh->generateNormals();
    }

    void Mesh::generatePlane(Mesh* mesh)
    {

        SubMesh subMesh;
        subMesh.indexCount = 6;
        subMesh.indexOffset = 0;
        subMesh.vertexCount = 4;
        subMesh.vertexOffset = 0;
        subMesh.name = mesh->name;
        subMesh.index = 0;

        mesh->subMeshes.push_back(subMesh);

        //Bottom
        mesh->vertices.push_back({ {-1.0f,-1.0f,-1.0f}, { 0.0f,-1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f} });
        mesh->vertices.push_back({ { 1.0f,-1.0f,-1.0f}, { 0.0f,-1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f} });
        mesh->vertices.push_back({ { 1.0f,-1.0f, 1.0f}, { 0.0f,-1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f} });
        mesh->vertices.push_back({ {-1.0f,-1.0f, 1.0f}, { 0.0f,-1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f} });

        mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(2);
        mesh->indices.push_back(2); mesh->indices.push_back(3); mesh->indices.push_back(0);

        mesh->generateTangentBitangent();
        mesh->generateNormals();
    }

    void Mesh::generateCylinder(Mesh* mesh)
    {
       /* SubMesh subMesh;
        subMesh.index = 0;
        subMesh.indexCount;
        subMesh.indexOffset = 0;
        subMesh.vertexCount = 0;
        subMesh.vertexOffset = 0;
        subMesh.name = mesh->name;

        mesh->subMeshes.push_back(subMesh);

        float radius = 1.0f;
        float halfHeight = 2.0f;

        int capSlices = 30;

        std::vector<glm::vec3>topPos;
        for (int i = 0; i < capSlices; i++)
        {
            topPos.push_back({ radius * glm::cos(i), halfHeight, radius * glm::sin(i) });
        }

        for (int i = 0; i < capSlices; i++)
        {
            topPos.push_back({ radius * glm::cos(i), -halfHeight, radius * glm::sin(i) });
        }

        glm::vec3 center = { 0.0f,halfHeight,0.0f };
        for (int i = 0; i < mesh->vertices.size(); i++)
        {
            if (i == 29)
                center = { 0.0,-halfHeight,0.0 };

            glm::vec3 position{ center, topPos[i],topPos[i + 1] };
            glm::vec3 normal{};
            glm::vec2 uv{};

            glm::vec3 tangent{};
            glm::vec3 bitangent{};

            mesh->vertices.push_back({ position,normal,uv,tangent,bitangent });
        }*/
    }


	void Mesh::loadObjFile(filespace::filepath path)
	{
        tinyobj::ObjReaderConfig config;
        tinyobj::ObjReader reader;
        config.triangulate = true;

        if (!reader.ParseFromFile(path.string()))
        {
            if (!reader.Error().empty())
            {
                IAONNIS_LOG_ERROR("[TinyObj]: %s", reader.Error().c_str());
            }
            return;
        }
        if (!reader.Warning().empty())
        {
            IAONNIS_LOG_WARN("[TinyObj]: %s", reader.Warning().c_str());
        }

        const tinyobj::attrib_t& attrib = reader.GetAttrib();
        const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
        const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();

        subMeshes.resize(shapes.size());

        //Load Geometry Data
        for (size_t s = 0; s < shapes.size(); s++)
        {
            const auto& shape = shapes[s];

            subMeshes[s].name = shape.name;
            subMeshes[s].vertexOffset = vertices.size();
            subMeshes[s].indexOffset = indices.size();

            size_t startVertexCount = vertices.size();
            size_t startIndexCount = indices.size();

            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
            {
                int fv = shape.mesh.num_face_vertices[f];
                for (int v = 0; v < fv; v++)
                {
                    auto idx = shape.mesh.indices[index_offset + v];

                    if (idx.vertex_index < 0 || idx.vertex_index * 3 + 2 >= attrib.vertices.size())
                    {
                        IAONNIS_LOG_ERROR("[TinyObj]: Vertex index out of bounds");
                        continue;
                    }

                    glm::vec3 pos = {
                        attrib.vertices[3 * idx.vertex_index + 0],
                        attrib.vertices[3 * idx.vertex_index + 1],
                        attrib.vertices[3 * idx.vertex_index + 2]
                    };

                    glm::vec2 uv = { 0.0f, 0.0f };
                    if (idx.texcoord_index != -1)
                    {
                        if (idx.texcoord_index * 2 + 1 < attrib.texcoords.size())
                        {
                            uv = {
                                attrib.texcoords[2 * idx.texcoord_index + 0],
                                attrib.texcoords[2 * idx.texcoord_index + 1]
                            };
                        }
                    }

                    glm::vec3 normal{ 0.0f, 0.0f, 0.0f };
                    if (idx.normal_index != -1)
                    {
                        if (idx.normal_index * 3 + 2 < attrib.normals.size())
                        {
                            normal = {
                                attrib.normals[3 * idx.normal_index + 0],
                                attrib.normals[3 * idx.normal_index + 1],
                                attrib.normals[3 * idx.normal_index + 2]
                            };
                        }
                    }
                    else
                    {
                        IAONNIS_LOG_WARN("[TinyObj]: No normals provided for Sub Mesh %s", shape.name);
                        normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    }

                    vertices.push_back({ pos, normal, uv, glm::vec3(0.0f), glm::vec3(0.0f) });

                    size_t vertexIndex = vertices.size() - 1;
                    if (vertexIndex > std::numeric_limits<uint32_t>::max())
                    {
                        IAONNIS_LOG_ERROR("[TinyObj]: Too many vertices for index type");
                        continue;
                    }
                    indices.push_back(static_cast<uint32_t>(vertexIndex));
                }
                index_offset += fv;
            }

            subMeshes[s].vertexCount = vertices.size() - startVertexCount;
            subMeshes[s].indexCount = indices.size() - startIndexCount;
            subMeshes[s].index = s;
        }

        generateTangentBitangent();

        for (auto& material : materials)
        {
            SubMeshTexturePaths subMeshTexture;
            subMeshTexture.diffuseMap = material.diffuse_texname;
            subMeshTexture.normalMap = material.bump_texname;
            subMeshTexture.aoMap = material.ambient_texname;
            subMeshTexture.roughnessMap = material.specular_highlight_texname;
            subMeshTexture.metallicMap = material.metallic_texname;

            texturePaths.push_back(subMeshTexture);
        }

        IAONNIS_LOG_INFO("[TinyObj]: Loaded model with %d Sub Meshes, %d Vertices, %d Materials.",
            shapes.size(), vertices.size(), materials.size());
	}

    void Mesh::loadMeshFile(filespace::filepath path)
    {
        std::ifstream file(path.string(), std::ios::binary);
        if (!file.is_open())
        {
            IAONNIS_LOG_ERROR("Failed to read Mesh File");
            return;
        }

        MeshFileHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(MeshFileHeader));
        if (strcmp(header.magic, "IAONNIS"))
        {
            IAONNIS_LOG_ERROR("Invalid Mesh File. (Path = %s)", path.string());
            /*return*/;
        }
        std::vector<SubMeshHeader> subMeshHeaders(header.meshCount);
        std::vector<std::string> stringMap;

        file.read(reinterpret_cast<char*>(subMeshHeaders.data()), sizeof(SubMeshHeader) * header.meshCount);
        file.read(reinterpret_cast<char*>(vertices.data()), sizeof(Vertice) * header.vertexCount);
        file.read(reinterpret_cast<char*>(indices.data()), sizeof(uint32_t) * header.indexCount);
        file.read(reinterpret_cast<char*>(stringMap.data()), header.stringTableLength);

        subMeshes.resize(header.meshCount);

        int i = 0;
        for (auto& subMesh : subMeshHeaders)
        {
            std::string name = "";
            name.resize(subMesh.nameLength, 'c');
            _memccpy(name.data(), stringMap.data() + subMesh.nameOffset, 0, subMesh.nameLength);
            subMeshes[i].name = name;

            subMeshes[i].vertexCount  = subMesh.vertexCount;
            subMeshes[i].vertexOffset = subMesh.vertexOffset;

            subMeshes[i].indexCount  = subMesh.indexCount;
            subMeshes[i].indexOffset = subMesh.indexOffset;
        }
    }

    void Mesh::saveMeshFile(filespace::filepath path)
    {
        std::ofstream file(path.string(), std::ios::binary);
        if (!file.is_open())
        {
            IAONNIS_LOG_ERROR("Failed to write Mesh File");
            return;
        }

        MeshFileHeader header;
        header.meshCount = subMeshes.size();
        header.stringTableOffset = sizeof(MeshFileHeader)
            + (sizeof(SubMeshHeader) * header.meshCount)
            + (vertices.size() * sizeof(Vertice))
            + (indices.size() * sizeof(uint32_t));

        header.vertexCount = vertices.size();
        header.indexCount  = indices.size();

        std::vector<SubMeshHeader> subMeshHeaders(header.meshCount);
        std::vector<std::string> stringTable(header.meshCount + 1);

        size_t namePtr = 0;
        stringTable[0] = name;
        namePtr += name.size();

        int i = 0;
        for (auto& subMesh : subMeshes)
        {
            subMeshHeaders[i].indexCount  = subMesh.indexCount;
            subMeshHeaders[i].indexOffset = subMesh.indexOffset;

            subMeshHeaders[i].vertexCount  = subMesh.vertexCount;
            subMeshHeaders[i].vertexOffset = subMesh.vertexOffset;

            subMeshHeaders[i].nameLength = subMesh.name.size();
            subMeshHeaders[i].nameOffset = namePtr;

            stringTable[i + 1] = subMesh.name;
            namePtr += subMesh.name.size();
            i++;
        }

        header.stringTableLength = namePtr;

        file.write(reinterpret_cast<char*>(&header), sizeof(header));
        file.write(reinterpret_cast<char*>(subMeshHeaders.data()), sizeof(SubMeshHeader) * subMeshHeaders.size());

        file.write(reinterpret_cast<char*>(vertices.data()), sizeof(Vertice) * vertices.size());
        file.write(reinterpret_cast<char*>(indices.data()), sizeof(uint32_t) * indices.size());
        file.write(reinterpret_cast<char*>(stringTable.data()), namePtr);

        return;
    }

    void Mesh::generateTangentBitangent()
    {
        for (int s = 0; s < subMeshes.size(); s++)
        {
            for (size_t i = subMeshes[s].indexOffset; i < subMeshes[s].indexOffset + subMeshes[s].indexCount; i += 3)
            {
                if (i + 2 >= indices.size()) break;

                uint32_t i0 = indices[i];
                uint32_t i1 = indices[i + 1];
                uint32_t i2 = indices[i + 2];

                if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size()) continue;

                glm::vec3 pos0 = vertices[i0].p;
                glm::vec3 pos1 = vertices[i1].p;
                glm::vec3 pos2 = vertices[i2].p;

                glm::vec2 uv0 = vertices[i0].uv;
                glm::vec2 uv1 = vertices[i1].uv;
                glm::vec2 uv2 = vertices[i2].uv;

                glm::vec3 deltaPos1 = pos1 - pos0;
                glm::vec3 deltaPos2 = pos2 - pos0;

                glm::vec2 deltaUV1 = uv1 - uv0;
                glm::vec2 deltaUV2 = uv2 - uv0;

                float det = deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x;
                if (std::abs(det) < 1e-6f) continue;

                float invDet = 1.0f / det;

                glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * invDet;
                glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * invDet;

                vertices[i0].tangent += tangent;
                vertices[i1].tangent += tangent;
                vertices[i2].tangent += tangent;

                vertices[i0].bitangent += bitangent;
                vertices[i1].bitangent += bitangent;
                vertices[i2].bitangent += bitangent;
            }

            for (size_t i = subMeshes[s].vertexOffset; i < subMeshes[s].vertexOffset + subMeshes[s].vertexCount; i++)
            {
                auto& vertex = vertices[i];

                vertex.tangent = glm::normalize(vertex.tangent - vertex.n * glm::dot(vertex.n, vertex.tangent));
                float sign = (glm::dot(glm::cross(vertex.n, vertex.tangent), vertex.bitangent) < 0.0f) ? -1.0f : 1.0f;
                vertex.bitangent = glm::cross(vertex.n, vertex.tangent) * sign;
            }
        }
    }

    void Mesh::generateNormals()
    {
        IAONNIS_LOG_WARN("No Normals have been generated yet");
        return;
    }



}
