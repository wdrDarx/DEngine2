#include "BoxColliderComponent.h"
#include "DEngine.h"

void BoxColliderComponent::OnBeginPlay()
{
	Super::OnBeginPlay();

}

void BoxColliderComponent::OnEndPlay()
{
	Super::OnEndPlay();

}

void BoxColliderComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	//draw debug cube
	GetOwner()->GetScene()->GetPipeline()->GetRenderer<DebugRenderer>()->DrawDebugCube(GetWorldPostition(), GetWorldRotation(), BoxSize * GetWorldTransform().scale, {1,0,0});
}

Ref<ColliderShape> BoxColliderComponent::CreateColliderShape()
{
	Ref<BoxColliderShape> box = MakeRef<BoxColliderShape>();
	box->SetBoxSize(BoxSize * GetWorldTransform().scale);
	return box;
}
