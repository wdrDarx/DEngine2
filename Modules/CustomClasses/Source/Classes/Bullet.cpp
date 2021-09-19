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

	m_PhysicsCallback.Assign([&](PhysicsActorEvent* event)
	{
// 		if (event->GetEventType() == PhysicsActorEventType::ONBEGINOVERLAP)
// 		{
// 			LogTemp("BULLET Overlap " + event->Hit.OtherColliderComponent->GetName());
// 			//GetScene()->DestroySceneObject(event->Hit.OtherColliderComponent->GetOwner());
// 			event->Hit.OtherColliderComponent->GetPhysicsActor()->AddForce(event->Hit.HitNormal * -1000.f, ForceMode::VelocityChange);
// 		}
	});

	Root->GetPhysicsActor()->BindOnBeginOverlap(m_PhysicsCallback);

	Root->GetPhysicsActor()->AddForce(World::GetForwardVector(Root->GetWorldRotation()) * Speed, ForceMode::VelocityChange);
}
