#pragma once
#include "Core/Core.h"
#include "Utils/PhysicsUtils.h"
#include "Rendering/Mesh.h"
#include <PhysX/PxPhysicsAPI.h>

struct DENGINE_API CookingData
{
	physx::PxCooking* CookingSDK;
	physx::PxCookingParams CookingParameters;

	CookingData(const physx::PxTolerancesScale& scale) : CookingSDK(nullptr), CookingParameters(scale)
	{
		
	}
};

struct DENGINE_API MeshColliderData
{
	Buffer Data;
	//glm::mat4 Transform;
	bool IsConvex;
};

class PhysicsWorld;
class DENGINE_API CookingFactory
{
public:
	CookingFactory(PhysicsWorld* world);
	virtual ~CookingFactory();

public:

	PhysicsWorld* GetPhysicsWorld() const
	{
		return m_PhysicsWorld;
	}

	Ref<CookingData> m_CookingData;
	PhysicsWorld* m_PhysicsWorld;

	CookingResult CookMesh(Mesh& MeshToCook, const std::string& CacheName, bool CookConvex = false, MeshColliderData& outData = MeshColliderData());

	CookingResult CookConvexMesh(const Mesh* mesh, MeshColliderData& outData);
	CookingResult CookTriangleMesh(const Mesh* mesh, MeshColliderData& outData);

private:
	void GenerateDebugMesh(Mesh& mesh, const MeshColliderData& colliderData);
};

