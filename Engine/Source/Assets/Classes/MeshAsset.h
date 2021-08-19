#pragma once
#include "Assets/Asset.h"
#include "Rendering/Mesh.h"
#include "Serialization/Buffer.h"

class DENGINE_API MeshAsset : public Asset
{
public:
	MeshAsset() : Asset()
	{

	}

	~MeshAsset()
	{

	}

	uint Serialize(Buffer& buffer) const override
	{
		STARTWRITE(buffer, Asset::Serialize(buffer))
		WRITEVEC(m_Verticies, sizeof(MeshVertex));
		WRITEVEC(m_Indicies, sizeof(uint));
		STOPWRITE();
	}

	uint Deserialize(const Buffer& buffer) override
	{
		STARTREAD(buffer, Asset::Deserialize(buffer))
		READVEC(m_Verticies, sizeof(MeshVertex));
		READVEC(m_Indicies, sizeof(uint));
		STOPREAD();
	}

	void LoadMesh(Ref<Mesh> mesh)
	{
		mesh->SetData(m_Verticies, m_Indicies);
	}

	const std::vector<MeshVertex>& GetVerticies() const
	{
		return m_Verticies;
	}

	const std::vector<uint>& GetIndicies() const
	{
		return m_Indicies;
	}

public:
	std::vector<MeshVertex> m_Verticies;
	std::vector<uint> m_Indicies;
};