#include "Bullet.h"

void Bullet::OnConstruct()
{
	Super::OnConstruct();

	mesh = CreateComponent<StaticMeshComponent>(ObjectInitializer::Module(this), "Mesh");
	SetRootComponent(mesh);
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

	if(!mesh->GetPhysicsActor()) return;

	m_PhysicsCallback.Assign([&](PhysicsActorEvent* event)
	{
 		if (event->GetEventType() == PhysicsActorEventType::ONBEGINOVERLAP)
 		{
// 			LogTemp("BULLET Overlap " + event->Hit.OtherColliderComponent->GetName());
// 			//GetScene()->DestroySceneObject(event->Hit.OtherColliderComponent->GetOwner());
// 			event->Hit.OtherColliderComponent->GetPhysicsActor()->AddForce(event->Hit.HitNormal * -1000.f, ForceMode::VelocityChange);
 		}
	});

	mesh->GetPhysicsActor()->BindOnBeginOverlap(m_PhysicsCallback);

	mesh->GetPhysicsActor()->AddForce(World::GetForwardVector(mesh->GetWorldRotation()) * Speed, ForceMode::VelocityChange);
}
