#pragma once
#include "Mesh.h"
#include "Assets/Classes/MeshAsset.h"
#include "Assets/Classes/MaterialAsset.h"

class Scene;
/*
	only difference is that this class is bound to a mesh and material asset
*/
class StaticMesh : public Mesh
{
	friend class MeshRenderer;

public:
	using Mesh::Mesh;

	void SetStaticMesh(const AssetRef<MeshAsset>& meshAsset)
	{
		//compare asset ID's
		if (meshAsset != m_MeshAsset)
		{ 
			m_FlagChanged = true;
			if (meshAsset.IsValid())
				m_MeshAsset = meshAsset;
		}
	}

	void SetStaticMaterial(const AssetRef<MaterialAsset>& materialAsset)
	{
		//compare asset ID's
		if (materialAsset != m_MaterialAsset)
		{ 
			m_FlagChanged = true;
			if (materialAsset.IsValid())
				m_MaterialAsset = materialAsset;
		}	
	}

	/*
		Warning : this auto resets the changed flag!
	*/
	bool IsFlaggedChanged()
	{	
		bool out = m_FlagChanged;
		m_FlagChanged = false;
		return out;
	}

	bool IsValid()
	{
		return m_MaterialAsset.IsValid() && m_MeshAsset.IsValid();
	}

private:
	
	//called by the renderer when generating draw calls
	void ReloadFromAssets(Scene* SceneContext);

	AssetRef<MaterialAsset> m_MaterialAsset;
	AssetRef<MeshAsset> m_MeshAsset;

	Ref<MeshAsset> m_LoadedMeshAsset;
	Ref<MaterialAsset> m_LoadedMaterialAsset;

	bool m_FlagChanged;
};