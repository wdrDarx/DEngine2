#include "ColliderComponent.h"
#include "DEngine.h"

void ColliderComponent::OnBeginPlay()
{
	Super::OnBeginPlay();

	m_PhysicsActor = MakeRef<PhysicsActor>(GetOwner()->GetScene()->GetPhysicsScene().get());
	m_PhysicsActor->SetDynamic(Dynamic);

	Transform WorldTrans = GetWorldTransform();
	m_PhysicsActor->CreateRigidActor(WorldTrans);
	m_TargetPhysicsPos = WorldTrans.pos;
	m_TargetPhysicsQuat = World::RotationDegreesToQuat(WorldTrans.rot);

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

	m_PhysicsActor->SetMass(Mass);
	m_PhysicsActor->SetLinearDrag(LinearDamping);
	m_PhysicsActor->SetAngularDrag(AngularDamping);

	m_PhysicsActor->SetLockFlag(ActorLockFlag::PositionX, LockPosX);
	m_PhysicsActor->SetLockFlag(ActorLockFlag::PositionY, LockPosY);
	m_PhysicsActor->SetLockFlag(ActorLockFlag::PositionZ, LockPosZ);
	m_PhysicsActor->SetLockFlag(ActorLockFlag::RotationX, LockRotX);
	m_PhysicsActor->SetLockFlag(ActorLockFlag::RotationY, LockRotY);
	m_PhysicsActor->SetLockFlag(ActorLockFlag::RotationZ, LockRotZ);

	GetOwner()->GetScene()->GetPhysicsScene()->AddActor(m_PhysicsActor);

	//assign and bind callback
	m_PhysicsActorCallback.Assign([&](PhysicsActorEvent* event)
	{
		if(event->GetEventType() == PhysicsActorEventType::ONADVANCE)
			OnPhysicsActorAdvance();
	});

	m_PhysicsActor->BindOnAdvance(m_PhysicsActorCallback);
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
			if(m_PendingTransform) //this means we wanna teleport so no need to interpolate
			{			
				m_PhysicsActor->SetPosition(m_PendingTransform->pos);
				m_PhysicsActor->SetRotation(m_PendingTransform->rot);
				m_RecieveNextTransform = false;
				Super::SetWorldTransform(m_PendingTransform.value());
			}
			else if(m_PhysicsActor->IsDynamic()) //static actors dont recieve position back, its 1 sided
			{
				if(Lerp && !World::IsNearlyZero(m_TargetPhysicsPos)) //we're lerping and the target position has been set
				{  
					//interpolate - (takes exactly 1 physics step to get to new location which is perfect timing)
					//also means the transform is 1 physics timestep behind but that shouldnt be an issue
					float PhysicsFrequency = 1.0f / GetOwner()->GetScene()->GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsSettings().FixedTimestep;

					NewTrans.pos = World::Lerp(NewTrans.pos, m_TargetPhysicsPos, PhysicsFrequency * tick.DeltaTime);
					quat FinalQuat = World::LerpQuat(World::RotationDegreesToQuat(NewTrans.rot), m_TargetPhysicsQuat, PhysicsFrequency * tick.DeltaTime);
					NewTrans.rot = World::QuatToRotationDegrees(FinalQuat);
				}
				else
				{
					NewTrans.pos = m_PhysicsActor->GetPosition();
					NewTrans.rot = m_PhysicsActor->GetRotation();
				}

				m_RecieveNextTransform = false;
				Super::SetWorldTransform(NewTrans);
			}
			m_PendingTransform = {};
			m_RecieveNextTransform = true;
		}
	}
}

void ColliderComponent::OnPhysicsActorAdvance()
{
	//assign targets
	if(Lerp)
	{ 
		m_TargetPhysicsPos = m_PhysicsActor->GetPosition();
		m_TargetPhysicsQuat = PhysicsUtils::FromPhysXQuat(m_PhysicsActor->GetPhysXActor()->getGlobalPose().q);
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
