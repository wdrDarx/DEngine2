#include "PhysicsActor.h"
#include "PhysicsWorld.h"
#include "PhysXAPI.h"
#include "PhysicsLayer.h"
#include "DEngine.h"
#include <glm/gtx/compatibility.hpp>

PhysicsActor::PhysicsActor(PhysicsScene* physicsScene) : m_PhysicsScene(physicsScene)
{
	
}

PhysicsActor::~PhysicsActor()
{
	for (auto& collider : m_Colliders)
	{
		collider->DetachFromActor(GetPhysXActor());
		collider->Release();
	}

	GetPhysicsScene()->GetPhysXScene()->removeActor(*GetPhysXActor());
	GetPhysXActor()->release();
	m_RigidActor = nullptr;
}

float PhysicsActor::GetMass() const
{
	return !IsDynamic() ? m_Mass : GetPhysXActor()->is<physx::PxRigidDynamic>()->getMass();
}

void PhysicsActor::SetMass(float mass)
{
	if (!IsDynamic())
	{
		LogWarning("Cannot set mass of non-dynamic PhysicsActor.");
		return;
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);

	GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsAPI()->setMassAndUpdateInertia(*actor, mass);
	m_Mass = mass;
}

void PhysicsActor::AddForce(const vec3d& force, ForceMode forceMode)
{
	if (!IsDynamic())
	{
		LogWarning("Trying to add force to non-dynamic PhysicsActor.");
		return;
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	actor->addForce(PhysicsUtils::ToPhysXVector(force), (physx::PxForceMode::Enum)forceMode);
}

void PhysicsActor::AddTorque(const vec3d& torque, ForceMode forceMode)
{
	if (!IsDynamic())
	{
		LogWarning("Trying to add torque to non-dynamic PhysicsActor.");
		return;
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	actor->addTorque(PhysicsUtils::ToPhysXVector(torque), (physx::PxForceMode::Enum)forceMode);
}

vec3d PhysicsActor::GetLinearVelocity() const
{
	if (!IsDynamic())
	{
		LogWarning("Trying to get velocity of non-dynamic PhysicsActor.");
		return vec3d(0.0f);
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	return PhysicsUtils::FromPhysXVector(actor->getLinearVelocity());
}

void PhysicsActor::SetLinearVelocity(const vec3d& velocity)
{
	if (!IsDynamic())
	{
		LogWarning("Trying to set velocity of non-dynamic PhysicsActor.");
		return;
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	actor->setLinearVelocity(PhysicsUtils::ToPhysXVector(velocity));
}

vec3d PhysicsActor::GetAngularVelocity() const
{
	if (!IsDynamic())
	{
		LogWarning("Trying to get angular velocity of non-dynamic PhysicsActor.");
		return vec3d(0.0f);
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	return PhysicsUtils::FromPhysXVector(actor->getAngularVelocity());
}

void PhysicsActor::SetAngularVelocity(const vec3d& velocity)
{
	if (!IsDynamic())
	{
		LogWarning("Trying to set angular velocity of non-dynamic PhysicsActor.");
		return;
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	actor->setAngularVelocity(PhysicsUtils::ToPhysXVector(velocity));
}

float PhysicsActor::GetMaxLinearVelocity() const
{
	if (!IsDynamic())
	{
		LogWarning("Trying to get max linear velocity of non-dynamic PhysicsActor.");
		return 0.0f;
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	return actor->getMaxLinearVelocity();
}

void PhysicsActor::SetMaxLinearVelocity(float maxVelocity)
{
	if (!IsDynamic())
	{
		LogWarning("Trying to set max linear velocity of non-dynamic PhysicsActor.");
		return;
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	actor->setMaxLinearVelocity(maxVelocity);
}

float PhysicsActor::GetMaxAngularVelocity() const
{
	if (!IsDynamic())
	{
		LogWarning("Trying to get max angular velocity of non-dynamic PhysicsActor.");
		return 0.0f;
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	return actor->getMaxAngularVelocity();
}

void PhysicsActor::SetMaxAngularVelocity(float maxVelocity)
{
	if (!IsDynamic())
	{
		LogWarning("Trying to set max angular velocity of non-dynamic PhysicsActor.");
		return;
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	actor->setMaxAngularVelocity(maxVelocity);
}

void PhysicsActor::SetLinearDrag(float drag) const
{
	if (!IsDynamic())
	{
		LogWarning("Trying to set linear drag of non-dynamic PhysicsActor.");
		return;
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	actor->setLinearDamping(drag);
}

void PhysicsActor::SetAngularDrag(float drag) const
{
	if (!IsDynamic())
	{
		LogWarning("Trying to set angular drag of non-dynamic PhysicsActor.");
		return;
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	actor->setAngularDamping(drag);
}

vec3d PhysicsActor::GetKinematicTargetPosition() const
{
	if (!IsKinematic())
	{
		LogWarning("Trying to set kinematic target for a non-kinematic actor.");
		return vec3d(0.0f, 0.0f, 0.0f);
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	physx::PxTransform target;
	actor->getKinematicTarget(target);
	return PhysicsUtils::FromPhysXVector(target.p);
}

vec3d PhysicsActor::GetKinematicTargetRotation() const
{
	if (!IsKinematic())
	{
		LogWarning("Trying to set kinematic target for a non-kinematic actor.");
		return vec3d(0.0f, 0.0f, 0.0f);
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	physx::PxTransform target;
	actor->getKinematicTarget(target);
	return World::QuatToRotationDegrees(PhysicsUtils::FromPhysXQuat(target.q));
}

void PhysicsActor::SetKinematicTarget(const vec3d& targetPosition, const vec3d& targetRotation) const
{
	if (!IsKinematic())
	{
		LogWarning("Trying to set kinematic target for a non-kinematic actor.");
		return;
	}

	physx::PxRigidDynamic* actor = GetPhysXActor()->is<physx::PxRigidDynamic>();
	ASSERT(actor);
	actor->setKinematicTarget(PhysicsUtils::ToPhysXTransform(targetPosition, targetRotation));
}

void PhysicsActor::SetSimulationData(uint layerId)
{
	const PhysicsLayer& layerInfo = GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsLayerManager()->GetLayer(layerId);

	physx::PxFilterData filterData;
	filterData.word0 = layerInfo.BitValue; //my layer
	filterData.word1 = m_BlockingLayers; //what layers i block
	filterData.word2 = m_OverlappingLayers; //what layers i overlap
	filterData.word3 = (uint)m_CollisionDetectionType; //my collision mode

	for (auto& collider : m_Colliders)
		collider->SetFilterData(filterData);
}

void PhysicsActor::SetKinematic(bool isKinematic)
{
	if (!IsDynamic())
	{
		LogWarning("Static PhysicsActor can't be kinematic.");
		return;
	}

	m_IsKinematic = isKinematic;
	GetPhysXActor()->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, isKinematic);
}

void PhysicsActor::SetGravityEnabled(bool enabled)
{
	GetPhysXActor()->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !enabled);
}

void PhysicsActor::SetLockFlag(ActorLockFlag flag, bool value)
{
	if (value)
		m_LockFlags |= (uint32_t)flag;
	else
		m_LockFlags &= ~(uint32_t)flag;

	if (!IsDynamic())
		return;
	
	GetPhysXActor()->is<physx::PxRigidDynamic>()->setRigidDynamicLockFlag(PhysicsUtils::ToPhysXActorLockFlag(flag), value);
}

#if CHANGE_TYPE_AT_RUNTIME
void PhysicsActor::SetBodyType(RigidBodyType type)
{
	if (type == m_BodyType)
		return;

	//copy all actor data into a new one
	auto sdk = GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsAPI()->GetPhysXSDK();
	physx::PxTransform transform = GetPhysXActor()->getGlobalPose();
	physx::PxRigidActor* actor = nullptr;

	if (type == RigidBodyType::Static)
	{
		actor = sdk->createRigidStatic(transform);

		actor->setActorFlags(GetPhysXActor()->getActorFlags());
		actor->userData = GetPhysXActor()->userData;

		// Copy Shapes
		uint shapeCount = GetPhysXActor()->getNbShapes();
		physx::PxShape** shapes = new physx::PxShape * [shapeCount];
		GetPhysXActor()->getShapes(shapes, shapeCount);

		for (uint i = 0; i < shapeCount; i++)
		{
			actor->attachShape(*shapes[i]);
			GetPhysXActor()->detachShape(*shapes[i]);
		}
	}
	else
	{
		const PhysicsSettings& settings = GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsSettings();

		physx::PxRigidDynamic* dynamicActor = sdk->createRigidDynamic(transform);

		dynamicActor->setLinearDamping(1.0f);
		dynamicActor->setAngularDamping(1.0f);

		dynamicActor->setSolverIterationCounts(settings.SolverIterations, settings.SolverVelocityIterations);

		dynamicActor->setActorFlags(GetPhysXActor()->getActorFlags());
		dynamicActor->userData = GetPhysXActor()->userData;

		// Copy Shapes
		uint shapeCount = GetPhysXActor()->getNbShapes();
		physx::PxShape** shapes = new physx::PxShape * [shapeCount];
		GetPhysXActor()->getShapes(shapes, shapeCount);

		for (uint i = 0; i < shapeCount; i++)
		{
			dynamicActor->attachShape(*shapes[i]);
			GetPhysXActor()->detachShape(*shapes[i]);
		}

		dynamicActor->setMass(GetMass());
		actor = dynamicActor;
	}

	GetPhysXActor()->release();
	m_RigidActor = actor;
}

#endif

void PhysicsActor::SetPosition(const vec3d& pos)
{
	physx::PxTransform transform = GetPhysXActor()->getGlobalPose();
	transform.p = PhysicsUtils::ToPhysXVector(pos);
	GetPhysXActor()->setGlobalPose(transform);
}

void PhysicsActor::SetRotation(const vec3d& rot)
{
	physx::PxTransform transform = GetPhysXActor()->getGlobalPose();
	transform.q = PhysicsUtils::ToPhysXQuat(World::RotationDegreesToQuat(rot));
	GetPhysXActor()->setGlobalPose(transform);
}

void PhysicsActor::SetCollisionDetectionType(const CollisionDetectionType& type)
{
	GetPhysXActor()->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, type == CollisionDetectionType::Continuous);
	GetPhysXActor()->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, type == CollisionDetectionType::ContinuousSpeculative);

	m_CollisionDetectionType = type;
}

void PhysicsActor::BindOnAdvance(Callback<PhysicsActorEvent>& callback)
{
	m_EventDispatcher.Bind(callback);
}

void PhysicsActor::OnAdvance()
{
	PhysicsActorEvent event;
	event.m_EventType = PhysicsActorEventType::ONADVANCE;
	m_EventDispatcher.Dispatch(event);
}

void PhysicsActor::CreateRigidActor(const Transform& WorldTransform)
{
	if(!m_PhysicsScene) return;

	auto sdk = GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsAPI()->GetPhysXSDK();
	
	GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsAPI()->Load();

	if (m_BodyType == RigidBodyType::Static)
	{
		m_RigidActor = sdk->createRigidStatic(PhysicsUtils::ToPhysXTransform(WorldTransform));
	}
	else
	{
		const PhysicsSettings& settings = GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsSettings();

		m_RigidActor = sdk->createRigidDynamic(PhysicsUtils::ToPhysXTransform(WorldTransform));

		SetLockFlag(ActorLockFlag::PositionX, false);
		SetLockFlag(ActorLockFlag::PositionY, false);
		SetLockFlag(ActorLockFlag::PositionZ, false);
		SetLockFlag(ActorLockFlag::RotationX, false);
		SetLockFlag(ActorLockFlag::RotationY, false);
		SetLockFlag(ActorLockFlag::RotationZ, false);

		GetPhysXActor()->is<physx::PxRigidDynamic>()->setSolverIterationCounts(settings.SolverIterations, settings.SolverVelocityIterations);
	
		SetMass(100.f);
	}

	//default layer is 1
	if (!GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsLayerManager()->IsLayerValid(GetCollisionLayer()))
		SetCollisionLayer(1);

	GetPhysXActor()->userData = this;
}

