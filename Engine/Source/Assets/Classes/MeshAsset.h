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

		MapBuffer map;
		MAPBUFFERWRITE(map, "m_Verticies", { WRITEVEC(m_Verticies, sizeof(MeshVertex)); } );
		MAPBUFFERWRITE(map, "m_Indicies", { WRITEVEC(m_Indicies, sizeof(uint)); } );
		MAPBUFFERWRITE(map, "m_MeshBounds", { WRITE(&m_MeshBounds, sizeof(Bound)); } );
	
		WRITEBUFFER(map.MakeBuffer());
		
		STOPWRITE();
	}

	uint Deserialize(const Buffer& buffer) override
	{
		STARTREAD(buffer, Asset::Deserialize(buffer))

		Buffer mapBuf;
		READBUFFER(mapBuf);
		MapBuffer map;
		map.FromBuffer(mapBuf);

		MAPBUFFERREAD(map, "m_Verticies", { READVEC(m_Verticies, sizeof(MeshVertex)); });
		MAPBUFFERREAD(map, "m_Indicies", { READVEC(m_Indicies, sizeof(uint)); });
		MAPBUFFERREAD(map, "m_MeshBounds", { READ(&m_MeshBounds, sizeof(Bound)); });

		STOPWRITE();
	}

	void SetVerticies(const std::vector<MeshVertex>& verts)
	{
		m_Verticies = verts;
		GenBounds();
	}

	void Setindicies(const std::vector<uint>& indicies) 
	{
		m_Indicies = indicies;
	}

	void LoadMesh(Ref<Mesh> mesh)
	{
		mesh->SetData(m_Verticies, m_Indicies);
	}

	const std::vector<MeshVertex>& GetVerticies() const
	{
		return m_Verticies;
	}

	void GenBounds()
	{
		vec3d MinPoint = vec3d(std::numeric_limits<float>::max());
		vec3d MaxPoint = vec3d(std::numeric_limits<float>::min());

		for (uint i = 0; i < m_Verticies.size(); i++)
		{
			MinPoint.x = MIN(MinPoint.x, m_Verticies[i].Pos.x);
			MinPoint.y = MIN(MinPoint.y, m_Verticies[i].Pos.y);
			MinPoint.z = MIN(MinPoint.z, m_Verticies[i].Pos.z);

			MaxPoint.x = MAX(MaxPoint.x, m_Verticies[i].Pos.x);
			MaxPoint.y = MAX(MaxPoint.y, m_Verticies[i].Pos.y);
			MaxPoint.z = MAX(MaxPoint.z, m_Verticies[i].Pos.z);
		}

		m_MeshBounds = {MinPoint, MaxPoint};
	}

	const Bound& GetBounds() const
	{
		return m_MeshBounds;
	}



	const std::vector<uint>& GetIndicies() const
	{
		return m_Indicies;
	}

public:
	Bound m_MeshBounds;
	std::vector<MeshVertex> m_Verticies;
	std::vector<uint> m_Indicies;
};