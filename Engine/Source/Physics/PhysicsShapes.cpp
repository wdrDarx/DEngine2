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

physx::PxShape* ColliderShape::AttachShape(PhysicsActor* parent)
{	
	m_Parent = parent;

	physx::PxGeometry* geom = CreateGeometry();
	if (!geom) return nullptr;

	Ref<PhysicsMaterial> material = MakeRef<PhysicsMaterial>();
	SetMaterial(material);
	m_Parent->GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsAPI()->Load();

	

	m_Shape = createExclusiveShape(m_Parent->GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsAPI()->GetPhysXSDK(), *GetParentActor()->GetPhysXActor(), *geom, GetMaterial());
	delete geom;

	m_Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
	m_Shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);

	return m_Shape;
}

BoxColliderShape::BoxColliderShape() : ColliderShape(ColliderType::Box)
{
	
}

physx::PxGeometry* BoxColliderShape::CreateGeometry()
{
	return new physx::PxBoxGeometry(GetBoxSize().x, GetBoxSize().y, GetBoxSize().z);
}

TriangleColliderShape::TriangleColliderShape() : ColliderShape(ColliderType::TriangleMesh)
{

}

physx::PxGeometry* TriangleColliderShape::CreateGeometry()
{
	if(!GetCollisionMesh()) return nullptr;

	MeshColliderData data;
	CookingResult result = m_Parent->GetPhysicsScene()->GetPhysicsWorld()->GetCookingFactory()->CookMesh(*GetCollisionMesh(), m_CacheName, false, data);
	ASSERT(data.Data.size() > 0);

	if (result != CookingResult::Success)
		return nullptr;

	physx::PxDefaultMemoryInputData input(data.Data.data(), data.Data.size());
	physx::PxTriangleMesh* trimesh = m_Parent->GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsAPI()->GetPhysXSDK()->createTriangleMesh(input);

	return new physx::PxTriangleMeshGeometry(trimesh, physx::PxMeshScale({m_Scale.x, m_Scale.y, m_Scale.z}));
}

ConvexColliderShape::ConvexColliderShape() : TriangleColliderShape()
{
	m_Type = ColliderType::ConvexMesh;
}

physx::PxGeometry* ConvexColliderShape::CreateGeometry()
{
	if (!GetCollisionMesh()) return nullptr;

	MeshColliderData data;
	CookingResult result = m_Parent->GetPhysicsScene()->GetPhysicsWorld()->GetCookingFactory()->CookMesh(*GetCollisionMesh(), m_CacheName, true, data);
	ASSERT(data.Data.size() > 0);

	if (result != CookingResult::Success)
		return nullptr;

	physx::PxDefaultMemoryInputData input(data.Data.data(), data.Data.size());
	physx::PxConvexMesh* convexMesh = m_Parent->GetPhysicsScene()->GetPhysicsWorld()->GetPhysicsAPI()->GetPhysXSDK()->createConvexMesh(input);

	physx::PxConvexMeshGeometry* convexGeometry = new physx::PxConvexMeshGeometry(convexMesh, physx::PxMeshScale({ m_Scale.x, m_Scale.y, m_Scale.z }));
	convexGeometry->meshFlags = physx::PxConvexMeshGeometryFlag::eTIGHT_BOUNDS;

	return convexGeometry;
}
