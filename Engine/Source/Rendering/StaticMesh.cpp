#include "StaticMesh.h"
#include "DEngine.h"

void StaticMesh::GenBounds()
{
	if (!GetLoadedMeshAsset()) return;

	vec3d NewMin = GetMatrix() * vec4d(GetLoadedMeshAsset()->GetBounds().min, 1.0f);
	vec3d NewMax = GetMatrix() * vec4d(GetLoadedMeshAsset()->GetBounds().max, 1.0f);

	//turn min and max back into AABB
	vec3d copyMin = NewMin;
	vec3d copyMax = NewMax;

	NewMin.x = MIN(copyMin.x, copyMax.x);
	NewMin.y = MIN(copyMin.y, copyMax.y);
	NewMin.z = MIN(copyMin.z, copyMax.z);

	NewMax.x = MAX(copyMin.x, copyMax.x);
	NewMax.y = MAX(copyMin.y, copyMax.y);
	NewMax.z = MAX(copyMin.z, copyMax.z);

	m_CachedBounds = { NewMin, NewMax };
}

void StaticMesh::ReloadFromAssets(Scene* SceneContext)
{
	m_LoadedMeshAsset = SceneContext->GetApplication()->GetAssetManager().LoadAsset(m_MeshAsset);
	m_LoadedMaterialAsset = SceneContext->GetApplication()->GetAssetManager().LoadAsset(m_MaterialAsset);

	//this where data is actually changed
	if(m_LoadedMeshAsset)
		SetData(m_LoadedMeshAsset->GetVerticies(), m_LoadedMeshAsset->GetIndicies());

	//create an instance of the material from the asset
	if(m_LoadedMaterialAsset)
	{ 
		Ref<Material> NewMaterial;
		NewMaterial = ToRef<Material>(Cast<Material>(SceneContext->GetApplication()->GetObjectRegistry().MakeObjectFromClassName(m_LoadedMaterialAsset->GetObjectClassName())));
		NewMaterial->SetSceneContext(SceneContext);
		NewMaterial->Initialize(ObjectInitializer(ConstructFlags::RANDOMID));

		//load material props
		m_LoadedMaterialAsset->LoadObjectProps(NewMaterial);

		//set material
		SetMaterial(NewMaterial);
	}
}
