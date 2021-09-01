#pragma once
#include "TransformComponent.h"
#include "Physics/PhysicsCore.h"

class DENGINE_API ColliderComponent : public TransformComponent
{
public:	
	OBJECT_CLASS_DEF(ColliderComponent, TransformComponent);
	OBJECT_PROPS_BEGIN()
		PROPS_CATEGORY(Collider)
			PROPDEF(CollisionLayer, EditAnywhere);
			PROPDEF(CCD, EditAnywhere);
			PROPDEF(GravityEnabled, EditAnywhere);
			PROPDEF(Dynamic, EditAnywhere);
			PROPDEF(Kinamtic, EditAnywhere);
			PROPDEF(IsTrigger, EditAnywhere);
			PROPDEF(LinearDamping, EditAnywhere);
			PROPDEF(AngularDamping, EditAnywhere);
			PROPDEF(Mass, EditAnywhere);
		PROPS_CATEGORY(Locks)
			PROPDEF(LockPosX, EditAnywhere);
			PROPDEF(LockPosY, EditAnywhere);
			PROPDEF(LockPosZ, EditAnywhere);
			PROPDEF(LockRotX, EditAnywhere);
			PROPDEF(LockRotX, EditAnywhere);
			PROPDEF(LockRotX, EditAnywhere);
	OBJECT_PROPS_END()

	void OnBeginPlay() override;
	void OnEndPlay() override;
	void OnDestroy() override;
	void OnUpdate(const Tick& tick) override;

	//force update the world transform for the collider
	virtual bool SetLocalTransform(const Transform& LocalTrans) override;

	//this is the geometry of the collider
	virtual Ref<ColliderShape> CreateColliderShape()
	{
		return nullptr;
	}

	Ref<PhysicsActor> GetPhysicsActor()
	{
		if(!m_PhysicsActor) LogWarning(GetName() + " Physics Actor is Null!");
		return m_PhysicsActor;
	}

	Ref<ColliderShape> GetColliderShape()
	{
		if (!m_Shape) LogWarning(GetName() + " Collider Shape is Null!");
		return m_Shape;
	}

	Ref<PhysicsActor> m_PhysicsActor;
	Ref<ColliderShape> m_Shape;
	uint CollisionLayer = 0;
	bool GravityEnabled = true;
	bool Dynamic = false;
	bool Kinamtic = false;
	bool IsTrigger = false;
	bool CCD = true;

	float LinearDamping = 1.0f;
	float AngularDamping = 1.0f;
	float Mass = 100.0f;

	//lock flags
	bool LockPosX = false;
	bool LockPosY = false;
	bool LockPosZ = false;
	bool LockRotX = false;
	bool LockRotY = false;
	bool LockRotZ = false;

private:
	bool m_RecieveNextTransform = true;
	std::optional<Transform> m_PendingTransform;
};

