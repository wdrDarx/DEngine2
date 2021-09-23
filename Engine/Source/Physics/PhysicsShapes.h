#pragma once
#include "Core/Core.h"
#include "PhysicsMaterial.h"
#include "Utils/PhysicsUtils.h"
#include "CookingFactory.h"


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

class Mesh;
class DENGINE_API TriangleColliderShape : public ColliderShape
{
public:
	TriangleColliderShape();
	physx::PxGeometry* CreateGeometry() override;

	void SetCollisionMesh(const Ref<Mesh>& meshAsset, const std::string& CacheName)
	{
		m_ColliderMesh = meshAsset;
		m_CacheName = CacheName;
	}

	void SetScale(const vec3d& scale)
	{
		m_Scale = scale;
	}

	Ref<Mesh> GetCollisionMesh() const
	{
		return m_ColliderMesh;
	}

protected:
	Ref<Mesh> m_ColliderMesh;
	std::string m_CacheName;
	vec3d m_Scale;
};

class DENGINE_API ConvexColliderShape : public TriangleColliderShape
{
public:
	ConvexColliderShape();
	physx::PxGeometry* CreateGeometry() override;
};
