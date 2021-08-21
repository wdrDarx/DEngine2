#include "StaticMeshComponent.h"

void StaticMeshComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	m_StaticMesh->SetTransform(GetWorldTransform());
	m_StaticMesh->SetStaticMaterial(m_MaterialAsset);
	m_StaticMesh->SetStaticMesh(m_MeshAsset);
}

void StaticMeshComponent::OnConstruct()
{
	Super::OnConstruct();

	m_StaticMesh = MakeRef<StaticMesh>();
	GetOwner()->GetScene()->GetRenderer<MeshRenderer>()->SubmitStaticMesh(m_StaticMesh);
}

void StaticMeshComponent::OnDestroy()
{
	Super::OnDestroy();

	GetOwner()->GetScene()->GetRenderer<MeshRenderer>()->RemoveStaticMesh(m_StaticMesh);
}
