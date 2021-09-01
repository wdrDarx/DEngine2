#include "PhysicsShapes.h"
#include "PhysXAPI.h"
#include "PhysicsActor.h"
#include "DEngine.h"


physx::PxShape* createExclusiveShape(physx::PxPhysics* sdk, physx::PxRigidActor& actor, const physx::PxGeometry& geometry, physx::PxMaterial& material,
	physx::PxShapeFlags shapeFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE)
{
	physx::PxShape* shape = sdk->createShape(geometry, material, true, shapeFlags);
	if (shape)
	{
		bool status = actor.attachShape(*shape);	// attach can fail, if e.g. we try and attach a trimesh simulation shape to a dynamic actor
		shape->release();		// if attach fails, we hold the only counted reference, and so this cleans up properly
		if (!status)
			shape = NULL;
	}
	return shape;
}

void ColliderShape::SetMaterial(Ref<PhysicsMaterial> material)
{
	if (m_Material != nullptr)
		m_Material->release();

	m_Material = m_Parent->GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsAPI()->GetPhysXSDK()->createMaterial(material->StaticFriction, material->DynamicFriction, material->Bounciness);
}

void ColliderShape::SetTrigger(bool isTrigger)
{
	m_Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
	m_Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
	m_Shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	m_IsTrigger = isTrigger;
}

physx::PxShape* ColliderShape::AttachShape(PhysicsActor* parent)
{	
	m_Parent = parent;

	Ref<PhysicsMaterial> material = MakeRef<PhysicsMaterial>();
	SetMaterial(material);
	m_Parent->GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsAPI()->Load();

	physx::PxGeometry* geom = CreateGeometry();
	m_Shape = createExclusiveShape(m_Parent->GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsAPI()->GetPhysXSDK(), *GetParentActor()->GetPhysXActor(), *geom, GetMaterial());

	return m_Shape;
}

BoxColliderShape::BoxColliderShape() : ColliderShape(ColliderType::Box)
{
	
}

physx::PxGeometry* BoxColliderShape::CreateGeometry()
{
	return new physx::PxBoxGeometry(GetBoxSize().x, GetBoxSize().y, GetBoxSize().z);
}
