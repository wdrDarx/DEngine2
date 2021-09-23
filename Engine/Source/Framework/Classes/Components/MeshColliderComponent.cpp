#include "MeshColliderComponent.h"
#include "DEngine.h"

void MeshColliderComponent::OnBeginPlay()
{
	Super::OnBeginPlay();
}

void MeshColliderComponent::OnEndPlay()
{
	Super::OnEndPlay();
}

void MeshColliderComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
}

Ref<ColliderShape> MeshColliderComponent::CreateColliderShape()
{
	bool convex = Convex;
	if(Dynamic && Convex)
		convex = true; //dynamic mesh cant be non-convex

	Ref<TriangleColliderShape> collider = convex ? MakeRef<ConvexColliderShape>() : MakeRef<TriangleColliderShape>();
	auto mesh = GetOwner()->GetScene()->GetApplication()->GetAssetManager().LoadAsset(ColliderMesh);

	if (mesh)
	{
		m_LoadedMesh = MakeRef<Mesh>();
		mesh->LoadMesh(m_LoadedMesh);
		collider->SetCollisionMesh(m_LoadedMesh, ColliderMesh.GetAssetHandle()->GetAssetName());
		collider->SetScale(GetWorldTransform().scale);
	}
	
	return collider;
}
