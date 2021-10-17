#pragma once
#include "ModelLoader.h"
#include "ObjLoader.h"
#include "DEngine.h"
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include <Assimp/Importer.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

inline void hash_combine(std::size_t& seed) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	hash_combine(seed, rest...);
}

namespace std 
{
	template<> 
	struct hash<MeshVertex>
	{
		size_t operator()(MeshVertex const& vertex) const 
		{
			std::size_t hash;
			hash_combine(hash, vertex.Pos, vertex.Norm, vertex.TexCoord);
			return hash;
		}
	};
}

void ModelLoader::LoadOBJ(const std::string& fullpath, Mesh* mesh)
{
	objl::Loader loader;
	loader.LoadFile(fullpath);

	auto m = loader.LoadedMeshes[0];

	std::vector<MeshVertex> verts;
	std::unordered_map<MeshVertex, uint> uniqueVertices;
	std::vector<uint> indicies;

	for (uint& i : m.Indices)
	{
		const auto& v = m.Vertices[i];

		glm::vec3 pos = { v.Position.X,  v.Position.Y,  v.Position.Z };
		glm::vec3 norm = { v.Normal.X,  v.Normal.Y,  v.Normal.Z };
		glm::vec2 coords = { v.TextureCoordinate.X, v.TextureCoordinate.Y };

		MeshVertex vertex{ pos, norm, {0,0,0}, {0,0,0}, coords };

		if (uniqueVertices.count(vertex) == 0)
		{
			uniqueVertices[vertex] = static_cast<uint>(verts.size());
			verts.push_back(vertex);
		}

		indicies.push_back(uniqueVertices[vertex]);
	}

	mesh->SetData(verts, indicies);
}

void ModelLoader::LoadFBX(const std::string& fullpath, Mesh* mesh)
{
		const uint MeshImportFlags =
		aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
		aiProcess_Triangulate |             // Make sure we're triangles
		aiProcess_SortByPType |             // Split meshes by primitive type
		aiProcess_GenNormals |              // Make sure we have legit normals
		aiProcess_GenUVCoords |             // Convert UVs if required 
		aiProcess_OptimizeMeshes |          // Batch draws where possible
		aiProcess_JoinIdenticalVertices |
		aiProcess_ValidateDataStructure;    // Validation

	using namespace Assimp;
	Importer importer;
	const aiScene* scene = importer.ReadFile(fullpath.c_str(), MeshImportFlags);
	auto Importedmesh = scene->mMeshes[0];
	auto verts = Importedmesh->mVertices;

	std::vector<MeshVertex> ExportVerts;
	std::vector<uint> ExportIndicies;

	// Walk through each of the mesh's vertices
	for (uint i = 0; i < Importedmesh->mNumVertices; i++)
	{
		MeshVertex vertex;
		glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

		// Positions
		vector.x = Importedmesh->mVertices[i].x;
		vector.y = Importedmesh->mVertices[i].y;
		vector.z = Importedmesh->mVertices[i].z;
		vertex.Pos = vector;

		// Normals
		vector.x = Importedmesh->mNormals[i].x;
		vector.y = Importedmesh->mNormals[i].y;
		vector.z = Importedmesh->mNormals[i].z;
		vertex.Norm = vector;

		//tangent
		vector.x = Importedmesh->mTangents[i].x;
		vector.y = Importedmesh->mTangents[i].y;
		vector.z = Importedmesh->mTangents[i].z;
		vertex.Tangent = vector;

		//bitangent
		vector.x = Importedmesh->mBitangents[i].x;
		vector.y = Importedmesh->mBitangents[i].y;
		vector.z = Importedmesh->mBitangents[i].z;
		vertex.Bitangent  = vector;


		// Texture Coordinates
		if (Importedmesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = Importedmesh->mTextureCoords[0][i].x;
			vec.y = Importedmesh->mTextureCoords[0][i].y;
			vertex.TexCoord = vec;
		}
		else
		{
			vertex.TexCoord = glm::vec2(0.0f, 0.0f);
		}

		ExportVerts.push_back(vertex);
	}

	// Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < Importedmesh->mNumFaces; i++)
	{
		aiFace face = Importedmesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			ExportIndicies.push_back(face.mIndices[j]);
		}
	}

	mesh->SetData(ExportVerts, ExportIndicies);
}
