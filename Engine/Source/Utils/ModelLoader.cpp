#pragma once
#include "ModelLoader.h"
#include "ObjLoader.h"
#include "DEngine.h"
// #include <Assimp/scene.h>
// #include <Assimp/postprocess.h>
// #include "Assimp/Importer.hpp"

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

		MeshVertex vertex{ pos, norm, coords };

		if (uniqueVertices.count(vertex) == 0)
		{
			uniqueVertices[vertex] = static_cast<uint>(verts.size());
			verts.push_back(vertex);
		}

		indicies.push_back(uniqueVertices[vertex]);
	}

	mesh->SetData(verts, indicies);
}

// void ModelLoader::LoadFBX(const std::string& fullpath, StaticMesh* mesh)
// {
// 	using namespace Assimp;
// 	Importer importer;
// 	const aiScene* scene = importer.ReadFile(fullpath.c_str(), aiProcess_Triangulate | aiProcess_CalcTangentSpace);
// 	auto Importedmesh = scene->mMeshes[0];
// 	auto verts = Importedmesh->mVertices;
// 
// 	std::vector<MeshVertex> ExportVerts;
// 	std::unordered_map<MeshVertex, uint> uniqueVertices;
// 	std::vector<uint> indicies;
// 
// 	for (uint i = 0; i < Importedmesh->mNumFaces; i++)
// 	{
// 		auto& face = Importedmesh->mFaces[i];
// 
// 		for (uint j = 0; j < face.mNumIndices; j++)
// 		{
// 			uint index = face.mIndices[j];
// 
// 			auto& vert = verts[index];
// 			auto& normal = Importedmesh->mNormals[index];
// 			auto& tan = Importedmesh->mTangents[index];
// 			auto& bitan = Importedmesh->mBitangents[index];
// 			auto& UVs = Importedmesh->mTextureCoords[0][index];
// 
// 			glm::vec3 pos = { vert.x,  vert.y,  vert.z };
// 			glm::vec3 norm = { normal.x,   normal.y,   normal.z };
// 			glm::vec3 tangent;
// 			glm::vec3 bitangent;
// 
// 			if (Importedmesh->HasTangentsAndBitangents())
// 			{
// 				tangent = { tan.x,   tan.y,   tan.z };
// 				bitangent = { bitan.x,   bitan.y,   bitan.z };
// 			}
// 
// 			glm::vec2 coords{ 0,0 };
// 			coords.x = UVs.x;
// 			coords.y = UVs.y;
// 			MeshVertex meshVertex{ pos, norm, tangent, bitangent, coords, {1,1,1,1} };
// 
// 			if (uniqueVertices.count(meshVertex) == 0)
// 			{
// 				uniqueVertices[meshVertex] = index;
// 				ExportVerts.push_back(meshVertex);
// 			}
// 
// 			indicies.push_back(uniqueVertices[meshVertex]);
// 		}
// 	}
// 
// 	mesh->SetData(ExportVerts, indicies);
// }
