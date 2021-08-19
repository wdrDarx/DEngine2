#include "MeshComponent.h"

void MeshComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
	
	m_Mesh->SetTransform(MeshTransform);

	auto mat = GetOwner()->GetScene()->GetApplication()->GetAssetManager().LoadAsset(m_MaterialAsset);
	auto mesh = GetOwner()->GetScene()->GetApplication()->GetAssetManager().LoadAsset(m_MeshAsset);

	if (mat && m_LastMaterialAssetID != mat->GetID())
	{
		m_Material = ToRef<Material>(Cast<Material>(GetOwner()->GetScene()->GetApplication()->GetObjectRegistry().MakeObjectFromClassName(mat->GetObjectClassName())));
		m_Material->SetSceneContext(GetOwner()->GetScene());
		m_Material->Initialize(ObjectInitializer(ConstructFlags::RANDOMID));
		m_Mesh->SetMaterial(m_Material);

		m_LastMaterialAssetID = mat->GetID();
	}

	if (mat)
	{
		mat->LoadObjectProps(m_Material);
	}

	if(mesh && m_LastMeshAssetID != mesh->GetID())
	{ 
		mesh->LoadMesh(m_Mesh);

		m_LastMeshAssetID = mesh->GetID();
	}
}

void MeshComponent::OnConstruct()
{
	Super::OnConstruct();

	m_Mesh = MakeRef<Mesh>();
	GetOwner()->GetScene()->GetRenderer<MeshRenderer>()->SubmitMesh(m_Mesh);
}

void MeshComponent::OnDestroy()
{
	Super::OnDestroy();

	GetOwner()->GetScene()->GetRenderer<MeshRenderer>()->RemoveMesh(m_Mesh);
}
