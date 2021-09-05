#include "StaticMeshComponent.h"

void StaticMeshComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
	MeshFlags flags = (MeshFlags)0;

	if(!CastShadow)
		flags |= MeshFlags::NOSHADOW;

	if(!Visible)
		flags |= MeshFlags::INVISIBLE;

	m_StaticMesh->SetMeshFlags(flags);
	m_StaticMesh->SetTransform(GetWorldTransform());
	m_StaticMesh->SetStaticMaterial(m_MaterialAsset);
	m_StaticMesh->SetStaticMesh(m_MeshAsset);
}

void StaticMeshComponent::OnConstruct()
{
	Super::OnConstruct();

	m_StaticMesh = MakeRef<StaticMesh>();
	GetOwner()->GetScene()->GetPipeline()->GetRenderer<MeshRenderer>()->SubmitStaticMesh(m_StaticMesh);
}

void StaticMeshComponent::OnDestroy()
{
	Super::OnDestroy();

	GetOwner()->GetScene()->GetPipeline()->GetRenderer<MeshRenderer>()->RemoveStaticMesh(m_StaticMesh);
}
