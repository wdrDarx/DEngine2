#pragma once
#include "Core/Core.h"
#include "Utils/PhysicsUtils.h"
#include "PhysicsShapes.h"
#include "Event/EventDispatcher.h"
#include "Event/PhysicsActorEvent.h"
#include "Utils/World.h"

//can shapes and collision stuff be changed on runtime
#define CHANGE_TYPE_AT_RUNTIME 1

enum class RigidBodyType 
{
	Static, 
	Dynamic
};

class PhysicsScene;
class PhysicsActor 
{
public:
	PhysicsActor(PhysicsScene* physicsScene);
	void CreateRigidActor(const Transform& WorldTransform);
	virtual ~PhysicsActor();

	float GetMass() const;
	void SetMass(float mass);

	void AddForce(const vec3d& force, ForceMode forceMode);
	void AddTorque(const vec3d& torque, ForceMode forceMode);

	vec3d GetLinearVelocity() const;
	void SetLinearVelocity(const vec3d& velocity);
	vec3d GetAngularVelocity() const;
	void SetAngularVelocity(const vec3d& velocity);

	float GetMaxLinearVelocity() const;
	void SetMaxLinearVelocity(float maxVelocity);
	float GetMaxAngularVelocity() const;
	void SetMaxAngularVelocity(float maxVelocity);

	void SetLinearDrag(float drag) const;
	void SetAngularDrag(float drag) const;

	vec3d GetKinematicTargetPosition() const;
	vec3d GetKinematicTargetRotation() const;
	void SetKinematicTarget(const vec3d& targetPosition, const vec3d& targetRotation) const;

	void SetSimulationData(uint layerId);

	bool IsDynamic() const
	{ 
		return m_BodyType == RigidBodyType::Dynamic; 
	}

	void SetDynamic(bool dynamic)
	{
		m_BodyType = dynamic ? RigidBodyType::Dynamic : RigidBodyType::Static;
	}

	bool IsKinematic() const 
	{ 
		return IsDynamic() && m_IsKinematic;
	}

	void SetKinematic(bool isKinematic);
	void SetGravityEnabled(bool enabled);

	bool IsGravityEnabled() const 
	{ 
		return !m_RigidActor->getActorFlags().isSet(physx::PxActorFlag::eDISABLE_GRAVITY); 
	}

	bool GetLockFlag(ActorLockFlag flag) const 
	{ 
		return (uint32_t)flag & m_LockFlags; 
	}

	void SetLockFlag(ActorLockFlag flag, bool value);
	void SetBodyType(RigidBodyType type);

	vec3d GetPosition() const 
	{
		return PhysicsUtils::FromPhysXVector(GetPhysXActor()->getGlobalPose().p);
	}

	vec3d GetRotation() const
	{
		return World::QuatToRotationDegrees(PhysicsUtils::FromPhysXQuat(GetPhysXActor()->getGlobalPose().q));
	}

	void SetPosition(const vec3d& pos);
	void SetRotation(const vec3d& rot);

	void AddCollider(Ref<ColliderShape> collider)
	{
		auto shape = collider->AttachShape(this);
		m_Colliders.push_back(collider);
	}

	physx::PxRigidActor* GetPhysXActor() const 
	{ 
		return m_RigidActor;
	}

	PhysicsScene* GetPhysicsScene()
	{
		return m_PhysicsScene;
	}

	const CollisionDetectionType& GetCollisionDetectionType()
	{
		return m_CollisionDetectionType;
	}

	uint GetCollisionLayer() const
	{
		return m_CollisionLayer;
	}

	void SetCollisionLayer(uint layer)
	{
		m_CollisionLayer = layer;
	}

	void SetBlockLayer(int LayerBit)
	{
		m_BlockingLayers |= LayerBit;
	}

	void SetOverlapLayer(int LayerBit)
	{
		m_OverlappingLayers |= LayerBit;
	}

	void SetIgnoreLayer(int LayerBit)
	{
		m_BlockingLayers &= ~LayerBit;
		m_OverlappingLayers &= ~LayerBit;
	}

	void SetCollisionDetectionType(const CollisionDetectionType& type);

	void BindOnAdvance(Callback<PhysicsActorEvent>& callback);
	void OnAdvance();

private:
	std::vector<Ref<ColliderShape>> m_Colliders;
	uint m_LockFlags = 0;
	uint m_CollisionLayer = 0;

	int m_BlockingLayers = 0;
	int m_OverlappingLayers = 0;

	RigidBodyType m_BodyType;
	CollisionDetectionType m_CollisionDetectionType = CollisionDetectionType::Discrete;
	float m_Mass = 1.0f;
	bool m_GravityEnabled = true;
	bool m_IsKinematic = false;

	physx::PxRigidActor* m_RigidActor = nullptr;
	PhysicsScene* m_PhysicsScene = nullptr;

	//dispaches physics actor events
	EventDispatcher m_EventDispatcher;
private:
	friend class PhysicsScene;
};

