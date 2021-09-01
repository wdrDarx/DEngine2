#pragma once
#include "Core/Core.h"
#include "Utils/PhysicsUtils.h"
#include <PhysX/PxPhysicsAPI.h>

struct CookingData
{
	physx::PxCooking* CookingSDK;
	physx::PxCookingParams CookingParameters;

	CookingData(const physx::PxTolerancesScale& scale)
		: CookingSDK(nullptr), CookingParameters(scale)
	{
	}
};

struct MeshColliderData
{
	byte* Data;
	glm::mat4 Transform;
	uint32_t Size;
};

class PhysXAPI;
class DENGINE_API CookingFactory
{
public:
	CookingFactory(PhysXAPI* api);
	virtual ~CookingFactory();

public:
	Ref<CookingData> m_CookingData;

#if 0
	static CookingResult CookMesh(MeshColliderComponent& component, bool invalidateOld = false, std::vector<MeshColliderData>& outData = std::vector<MeshColliderData>());

	static CookingResult CookConvexMesh(const Ref<Mesh>& mesh, std::vector<MeshColliderData>& outData);
	static CookingResult CookTriangleMesh(const Ref<Mesh>& mesh, std::vector<MeshColliderData>& outData);

private:
	static void GenerateDebugMesh(MeshColliderComponent& component, const MeshColliderData& colliderData);
#endif
};

