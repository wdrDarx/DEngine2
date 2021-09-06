#include "Bullet.h"

void Bullet::OnConstruct()
{
	Super::OnConstruct();

	Root = CreateComponent<BoxColliderComponent>(ObjectInitializer::Module(this), "Root");
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


}

void Bullet::OnBeginPlay()
{
	Super::OnBeginPlay();

	Root->GetPhysicsActor()->AddForce(World::GetForwardVector(Root->GetWorldRotation()) * Speed, ForceMode::VelocityChange);
}
