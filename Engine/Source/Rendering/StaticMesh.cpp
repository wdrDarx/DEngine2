#include "StaticMesh.h"
#include "DEngine.h"

void StaticMesh::ReloadFromAssets(Scene* SceneContext)
{
	m_LoadedMeshAsset = SceneContext->GetApplication()->GetAssetManager().LoadAsset(m_MeshAsset);
	m_LoadedMaterialAsset = SceneContext->GetApplication()->GetAssetManager().LoadAsset(m_MaterialAsset);

	//this where data is actually changed
	SetData(m_LoadedMeshAsset->GetVerticies(), m_LoadedMeshAsset->GetIndicies());

	//create an instance of the material from the asset
	Ref<Material> NewMaterial;
	NewMaterial = ToRef<Material>(Cast<Material>(SceneContext->GetApplication()->GetObjectRegistry().MakeObjectFromClassName(m_LoadedMaterialAsset->GetObjectClassName())));
	NewMaterial->SetSceneContext(SceneContext);
	NewMaterial->Initialize(ObjectInitializer(ConstructFlags::RANDOMID));

	//load material props
	m_LoadedMaterialAsset->LoadObjectProps(NewMaterial);

	//set material
	SetMaterial(NewMaterial);
}
