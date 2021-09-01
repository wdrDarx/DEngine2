#pragma once
#include "Core/Core.h"
#include "PhysicsMaterial.h"
#include "Utils/PhysicsUtils.h"
#include "CookingFactory.h"

// TODO: Heightfield, Plane
enum class ColliderType
{
	Box, Sphere, Capsule, ConvexMesh, TriangleMesh
};

class PhysicsActor;
class DENGINE_API ColliderShape
{
protected:
	ColliderShape(ColliderType type) : m_Type(type)
	{
			
	}

public:
	virtual ~ColliderShape()
	{
	
	}

	virtual physx::PxGeometry* CreateGeometry() = 0;
	physx::PxShape* AttachShape(PhysicsActor* parent);

	void Release()
	{
		m_Material->release();
	}

	void SetMaterial(Ref<PhysicsMaterial> material);

	bool IsTrigger() const
	{
		return m_IsTrigger;
	}
	void SetTrigger(bool isTrigger);

	void SetFilterData(const physx::PxFilterData& filterData)
	{
		m_Shape->setSimulationFilterData(filterData);
	}

	void DetachFromActor(physx::PxRigidActor* actor)
	{
		actor->detachShape(*m_Shape);
	}

	physx::PxMaterial& GetMaterial() const 
	{ 
		return *m_Material; 
	}

	bool IsValid() const 
	{ 
		return m_Material != nullptr; 
	}

	PhysicsActor* GetParentActor()
	{
		ASSERT(m_Parent);
		return m_Parent;
	}

protected:
	physx::PxShape* m_Shape = nullptr;
	bool m_IsTrigger = false;
	PhysicsActor* m_Parent = nullptr;
	ColliderType m_Type;
	physx::PxMaterial* m_Material = nullptr;
};

class DENGINE_API BoxColliderShape : public ColliderShape
{
public:
	BoxColliderShape();
	physx::PxGeometry* CreateGeometry() override;

	void SetBoxSize(const vec3d& size)
	{
		m_BoxSize = size;
	}

	const vec3d& GetBoxSize() const
	{
		return m_BoxSize;
	}

private:
	vec3d m_BoxSize;
};
