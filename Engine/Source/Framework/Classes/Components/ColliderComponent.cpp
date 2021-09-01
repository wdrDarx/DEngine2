#include "ColliderComponent.h"
#include "DEngine.h"

void ColliderComponent::OnBeginPlay()
{
	Super::OnBeginPlay();

	m_PhysicsActor = MakeRef<PhysicsActor>(GetOwner()->GetScene()->GetPhysicsScene().get());
	m_PhysicsActor->SetDynamic(Dynamic);
	m_PhysicsActor->CreateRigidActor(GetWorldTransform());

	if (m_PhysicsActor->IsDynamic())
	{ 
		m_PhysicsActor->SetCollisionDetectionType(CCD ? CollisionDetectionType::Continuous : CollisionDetectionType::Discrete);
		m_PhysicsActor->SetKinematic(Kinamtic);
		m_PhysicsActor->SetGravityEnabled(GravityEnabled);
	}

	m_PhysicsActor->SetCollisionLayer(CollisionLayer);

	m_Shape = CreateColliderShape();
	m_PhysicsActor->AddCollider(m_Shape);
	
	m_Shape->SetTrigger(IsTrigger);

	m_PhysicsActor->SetMass(100.f);
	m_PhysicsActor->SetLinearDrag(LinearDamping);
	m_PhysicsActor->SetAngularDrag(AngularDamping);

	m_PhysicsActor->SetLockFlag(ActorLockFlag::PositionX, LockPosX);
	m_PhysicsActor->SetLockFlag(ActorLockFlag::PositionY, LockPosY);
	m_PhysicsActor->SetLockFlag(ActorLockFlag::PositionZ, LockPosZ);
	m_PhysicsActor->SetLockFlag(ActorLockFlag::RotationX, LockRotX);
	m_PhysicsActor->SetLockFlag(ActorLockFlag::RotationY, LockRotY);
	m_PhysicsActor->SetLockFlag(ActorLockFlag::RotationZ, LockRotZ);

	GetOwner()->GetScene()->GetPhysicsScene()->AddActor(m_PhysicsActor);
}

void ColliderComponent::OnEndPlay()
{
	Super::OnEndPlay();

	if (m_PhysicsActor && GetOwner()->GetScene()->GetPhysicsScene())
	{
		GetOwner()->GetScene()->GetPhysicsScene()->RemoveActor(m_PhysicsActor);
	}
}

void ColliderComponent::OnDestroy()
{
	if (m_PhysicsActor && GetOwner()->GetScene()->GetPhysicsScene())
	{
		GetOwner()->GetScene()->GetPhysicsScene()->RemoveActor(m_PhysicsActor);
	}
}

void ColliderComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	if(tick.GetTickGroup() == TickGroup::GAME && m_PhysicsActor)
	{
		Transform NewTrans = GetWorldTransform();
		if(m_PhysicsActor->IsKinematic())
		{
			m_PhysicsActor->SetKinematicTarget(NewTrans.pos, NewTrans.rot);
		}
		else
		{ 
			if(m_PendingTransform)
			{			
				m_PhysicsActor->SetPosition(m_PendingTransform->pos);
				m_PhysicsActor->SetRotation(m_PendingTransform->rot);
				m_RecieveNextTransform = false;
				Super::SetWorldTransform(m_PendingTransform.value());
			}
			else
			{ 
				NewTrans.pos = m_PhysicsActor->GetPosition();
				NewTrans.rot = m_PhysicsActor->GetRotation();
				m_RecieveNextTransform = false;
				Super::SetWorldTransform(NewTrans);
			}
			m_PendingTransform = {};
			m_RecieveNextTransform = true;
		}
	}
}

bool ColliderComponent::SetLocalTransform(const Transform& LocalTrans)
{
	if(!m_RecieveNextTransform)
		return Super::SetLocalTransform(LocalTrans);

	bool out = Super::SetLocalTransform(LocalTrans);
	if(out)
	{ 
		m_PendingTransform = GetWorldTransform();
	}
	return out;
}
