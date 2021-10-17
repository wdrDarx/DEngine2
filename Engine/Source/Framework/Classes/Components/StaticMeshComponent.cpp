#include "StaticMeshComponent.h"
#include "DEngine.h"

void StaticMeshComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
	MeshFlags flags = (MeshFlags)0;

	if (!CastShadow)
		flags |= MeshFlags::NOSHADOW;

	if (!Visible)
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

	//update stuff on asset save
	m_AssetCallback.Assign([&](AssetEvent* event)
	{
		if (event->GetEventType() == AssetEventType::ASSETSAVE)
		{
			if (event->GetAsset()->GetID() == m_MeshAsset.GetAssetHandle()->GetAssetID() || event->GetAsset()->GetID() == m_MaterialAsset.GetAssetHandle()->GetAssetID())
				m_StaticMesh->FlagChanged();
		}
	});

	GetOwner()->GetScene()->GetApplication()->GetAssetManager().GetEventDispatcher().Bind(m_AssetCallback);
}

void StaticMeshComponent::OnDestroy()
{
	Super::OnDestroy();

	GetOwner()->GetScene()->GetPipeline()->GetRenderer<MeshRenderer>()->RemoveStaticMesh(m_StaticMesh);
}
