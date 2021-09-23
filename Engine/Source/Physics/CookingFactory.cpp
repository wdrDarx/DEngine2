#include "DEngine.h"
#include "CookingFactory.h"

CookingFactory::CookingFactory(PhysicsWorld* world)
{
	m_CookingData = MakeRef<CookingData>(world->GetPhysicsAPI()->GetPhysXSDK()->getTolerancesScale());
	m_CookingData->CookingParameters.meshWeldTolerance = 0.1f;
	m_CookingData->CookingParameters.scale = world->GetPhysicsAPI()->m_PhysicsWorldScale;
	m_CookingData->CookingParameters.buildGPUData = true;
	m_CookingData->CookingParameters.meshPreprocessParams = physx::PxMeshPreprocessingFlag::eWELD_VERTICES;
	m_CookingData->CookingParameters.midphaseDesc = physx::PxMeshMidPhase::eBVH34;

	m_CookingData->CookingSDK = PxCreateCooking(PX_PHYSICS_VERSION, *world->GetPhysicsAPI()->GetFoundation(), m_CookingData->CookingParameters);
	ASSERT(m_CookingData->CookingSDK);
}

CookingFactory::~CookingFactory()
{
	m_CookingData->CookingSDK->release();
	m_CookingData->CookingSDK = nullptr;
	m_CookingData = nullptr;
}

CookingResult CookingFactory::CookMesh(Mesh& MeshToCook, const std::string& CacheName, bool CookConvex, MeshColliderData& outData)
{
	CookingResult result = CookingResult::Failure;
	File::MakePath(Paths::GetCacheDirectory() + "\\CookedColliders\\"); //will create a path if it doesnt exist already

	std::string FullCachePath = Paths::GetCacheDirectory() + "\\CookedColliders\\" + CacheName + (CookConvex ? ".ConvexCollider" : ".Collider");
	if (File::DoesFileExist(FullCachePath))
	{
		Buffer data;
		File::ReadFile(FullCachePath, data);

		STARTREAD(data, 0);
		READBUFFER(outData.Data);
		READ(&outData.IsConvex, sizeof(bool));

		return CookingResult::Success;
	}
	else
	{ 
 		result = CookConvex ? CookConvexMesh(&MeshToCook, outData) : CookTriangleMesh(&MeshToCook, outData);
		if(result == CookingResult::Failure) return result;

		Buffer data;
		STARTWRITE(data, 0);
		WRITEBUFFER(outData.Data);
		WRITE(&outData.IsConvex, sizeof(bool));

		File::WriteFile(FullCachePath, data);
	}
 	return result;
}

CookingResult CookingFactory::CookConvexMesh(const Mesh* mesh, MeshColliderData& outData)
{
	const auto& vertices = mesh->GetVerticies();
	const auto& indices = mesh->GetIndicies();

	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = vertices.size();
	convexDesc.points.stride = sizeof(MeshVertex);
	convexDesc.points.data = vertices.data();
	convexDesc.indices.count = indices.size();
	convexDesc.indices.data = indices.data();
	convexDesc.indices.stride = sizeof(uint);
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX | physx::PxConvexFlag::eSHIFT_VERTICES;

	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	if (!m_CookingData->CookingSDK->cookConvexMesh(convexDesc, buf, &result))
	{
		LogError("Failed to cook convex mesh !");
		return PhysicsUtils::FromPhysXCookingResult(result);
	}

	outData.Data.resize(buf.getSize());
	outData.IsConvex = true;
	memcpy(outData.Data.data(), buf.getData(), outData.Data.size());

	return CookingResult::Success;
}

CookingResult CookingFactory::CookTriangleMesh(const Mesh* mesh, MeshColliderData& outData)
{
	const auto& vertices = mesh->GetVerticies();
	const auto& indices = mesh->GetIndicies();

	physx::PxTriangleMeshDesc triangleDesc;
	triangleDesc.points.count = vertices.size();
	triangleDesc.points.stride = sizeof(MeshVertex);
	triangleDesc.points.data = vertices.data();
	triangleDesc.triangles.count = indices.size() / 3;
	triangleDesc.triangles.data = indices.data();
	triangleDesc.triangles.stride = sizeof(uint) * 3;

	physx::PxDefaultMemoryOutputStream buf;
	physx::PxTriangleMeshCookingResult::Enum result;
	if (!m_CookingData->CookingSDK->cookTriangleMesh(triangleDesc, buf, &result))
	{
		LogError("Failed to cook convex mesh !");
		return PhysicsUtils::FromPhysXCookingResult(result);
	}

	outData.Data.resize(buf.getSize());
	outData.IsConvex = false;
	memcpy(outData.Data.data(), buf.getData(), outData.Data.size());

	return CookingResult::Success;
}

void CookingFactory::GenerateDebugMesh(Mesh& mesh, const MeshColliderData& colliderData)
{
	physx::PxDefaultMemoryInputData input((byte*)colliderData.Data.data(), colliderData.Data.size());
	
	std::vector<MeshVertex> vertices;
	std::vector<uint> indices;

	if (colliderData.IsConvex)
	{
		physx::PxConvexMesh* convexMesh =  GetPhysicsWorld()->GetPhysicsAPI()->GetPhysXSDK()->createConvexMesh(input);

		// Magic values since we can't easily extract the vertex/index count from convexMesh
		vertices.reserve(100);
		indices.reserve(50 * 3);

		// Based On: https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/ThirdParty/PhysX3/NvCloth/samples/SampleBase/renderer/ConvexRenderMesh.cpp
		const uint32_t nbPolygons = convexMesh->getNbPolygons();
		const physx::PxVec3* convexVertices = convexMesh->getVertices();
		const physx::PxU8* convexIndices = convexMesh->getIndexBuffer();

		uint32_t nbVertices = 0;
		uint32_t nbFaces = 0;
		uint32_t vertCounter = 0;
		uint32_t indexCounter = 0;

		for (uint32_t i = 0; i < nbPolygons; i++)
		{
			physx::PxHullPolygon polygon;
			convexMesh->getPolygonData(i, polygon);
			nbVertices += polygon.mNbVerts;
			nbFaces += (polygon.mNbVerts - 2) * 3;

			uint32_t vI0 = vertCounter;
			for (uint32_t vI = 0; vI < polygon.mNbVerts; vI++)
			{
				MeshVertex& v = vertices.emplace_back();
				v.Pos = PhysicsUtils::FromPhysXVector(convexVertices[convexIndices[polygon.mIndexBase + vI]]);
				vertCounter++;
			}

			for (uint32_t vI = 1; vI < uint32_t(polygon.mNbVerts) - 1; vI++)
			{
				indices.push_back(uint(vI0));
				indices.push_back(uint(vI0 + vI + 10));
				indices.push_back(uint(vI0 + vI));
				indexCounter++;
				indexCounter++;
				indexCounter++;
			}
		}

		convexMesh->release();
	}
	else
	{
		physx::PxTriangleMesh* trimesh = GetPhysicsWorld()->GetPhysicsAPI()->GetPhysXSDK()->createTriangleMesh(input);

		const uint32_t nbVerts = trimesh->getNbVertices();
		const physx::PxVec3* triangleVertices = trimesh->getVertices();
		const uint32_t nbTriangles = trimesh->getNbTriangles();
		const physx::PxU16* tris = (const physx::PxU16*)trimesh->getTriangles();

		vertices.reserve(nbVerts);
		indices.reserve(nbTriangles);

		for (uint32_t v = 0; v < nbVerts; v++)
		{
			MeshVertex& v1 = vertices.emplace_back();
			v1.Pos = PhysicsUtils::FromPhysXVector(triangleVertices[v]);
		}

		for (uint32_t tri = 0; tri < nbTriangles; tri++)
		{
			indices.push_back(uint(tris[3 * tri + 0]));
			indices.push_back(uint(tris[3 * tri + 1]));
			indices.push_back(uint(tris[3 * tri + 2]));
		}

		trimesh->release();
	}

	mesh.SetData(vertices, indices);
}

