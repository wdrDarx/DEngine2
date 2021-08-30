#include "Bullet.h"

void Bullet::OnConstruct()
{
	Super::OnConstruct();

	Root = CreateComponent<TransformComponent>(ObjectInitializer::Module(this), "Root");
	mesh = CreateComponent<StaticMeshComponent>(ObjectInitializer::Module(this), "Mesh");

	mesh->AttachTo(Root);
	SetRootComponent(Root);
}

void Bullet::OnPostConstruct()
{
	Super::OnPostConstruct();
}

void Bullet::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	if(tick.GetTickGroup() != TickGroup::GAME) return;

	Root->AddWorldPostionOffset(World::GetForwardVector(Root->GetWorldRotation()) * Speed * tick.DeltaTime);
}
