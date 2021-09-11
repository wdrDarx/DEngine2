#include "PhysicsUtils.h"
#include "DEngine.h"

physx::PxTransform PhysicsUtils::ToPhysXTransform(const Transform& transform)
{
	physx::PxQuat r = ToPhysXQuat(World::RotationDegreesToQuat(transform.rot));
	physx::PxVec3 p = ToPhysXVector(transform.pos);
	return physx::PxTransform(p, r);
}

physx::PxTransform PhysicsUtils::ToPhysXTransform(const glm::mat4& transform)
{
	Transform trans = World::MakeTransform(transform);

	physx::PxQuat r = ToPhysXQuat(World::RotationDegreesToQuat(trans.rot));
	physx::PxVec3 p = ToPhysXVector(trans.pos);
	return physx::PxTransform(p, r);
}

physx::PxTransform PhysicsUtils::ToPhysXTransform(const vec3d& translation, const vec3d& rotation)
{
	return physx::PxTransform(ToPhysXVector(translation), ToPhysXQuat(World::RotationDegreesToQuat(rotation)));
}

physx::PxMat44 PhysicsUtils::ToPhysXMatrix(const glm::mat4& matrix) { return *(physx::PxMat44*)&matrix; }
const physx::PxVec3& PhysicsUtils::ToPhysXVector(const vec3d& vector) { return *(physx::PxVec3*)&vector; }
const physx::PxVec4& PhysicsUtils::ToPhysXVector(const vec4d& vector) { return *(physx::PxVec4*)&vector; }
physx::PxQuat PhysicsUtils::ToPhysXQuat(const glm::quat& quat) { return physx::PxQuat(quat.x, quat.y, quat.z, quat.w); }

glm::mat4 PhysicsUtils::FromPhysXTransform(const physx::PxTransform& transform)
{
	glm::quat rotation = FromPhysXQuat(transform.q);
	vec3d position = FromPhysXVector(transform.p);
	return glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation);
}

glm::mat4 PhysicsUtils::FromPhysXMatrix(const physx::PxMat44& matrix) { return *(glm::mat4*)&matrix; }
vec3d PhysicsUtils::FromPhysXVector(const physx::PxVec3& vector) { return *(glm::vec3*)&vector; }
vec4d PhysicsUtils::FromPhysXVector(const physx::PxVec4& vector) { return *(glm::vec4*)&vector; }
glm::quat PhysicsUtils::FromPhysXQuat(const physx::PxQuat& quat) { return glm::quat(quat.w, quat.x, quat.y, quat.z); }

CookingResult PhysicsUtils::FromPhysXCookingResult(physx::PxConvexMeshCookingResult::Enum cookingResult)
{
	switch (cookingResult)
	{
		case physx::PxConvexMeshCookingResult::eSUCCESS: return CookingResult::Success;
		case physx::PxConvexMeshCookingResult::eZERO_AREA_TEST_FAILED: return CookingResult::ZeroAreaTestFailed;
		case physx::PxConvexMeshCookingResult::ePOLYGONS_LIMIT_REACHED: return CookingResult::PolygonLimitReached;
		case physx::PxConvexMeshCookingResult::eFAILURE: return CookingResult::Failure;
	}

	return CookingResult::Failure;
}

CookingResult PhysicsUtils::FromPhysXCookingResult(physx::PxTriangleMeshCookingResult::Enum cookingResult)
{
	switch (cookingResult)
	{
		case physx::PxTriangleMeshCookingResult::eSUCCESS: return CookingResult::Success;
		case physx::PxTriangleMeshCookingResult::eLARGE_TRIANGLE: return CookingResult::LargeTriangle;
		case physx::PxTriangleMeshCookingResult::eFAILURE: return CookingResult::Failure;
	}

	return CookingResult::Failure;
}

physx::PxBroadPhaseType::Enum PhysicsUtils::ToPhysXBroadphaseType(BroadphaseType type)
{
	switch (type)
	{
		case BroadphaseType::SweepAndPrune: return physx::PxBroadPhaseType::eSAP;
		case BroadphaseType::MultiBoxPrune: return physx::PxBroadPhaseType::eMBP;
		case BroadphaseType::AutomaticBoxPrune: return physx::PxBroadPhaseType::eABP;
		case BroadphaseType::GPU: return physx::PxBroadPhaseType::eGPU;
	}

	return physx::PxBroadPhaseType::eABP;
}

physx::PxFrictionType::Enum PhysicsUtils::ToPhysXFrictionType(FrictionType type)
{
	switch (type)
	{
		case FrictionType::Patch:			return physx::PxFrictionType::ePATCH;
		case FrictionType::OneDirectional:	return physx::PxFrictionType::eONE_DIRECTIONAL;
		case FrictionType::TwoDirectional:	return physx::PxFrictionType::eTWO_DIRECTIONAL;
	}

	return physx::PxFrictionType::ePATCH;
}

physx::PxRigidDynamicLockFlag::Enum PhysicsUtils::ToPhysXActorLockFlag(ActorLockFlag flag)
{
	switch (flag)
	{
		case ActorLockFlag::PositionX: return physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
		case ActorLockFlag::PositionY: return physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
		case ActorLockFlag::PositionZ: return physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
		case ActorLockFlag::RotationX: return physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
		case ActorLockFlag::RotationY: return physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
		case ActorLockFlag::RotationZ: return physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
	}

	ASSERT(false);
	return physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
}

physx::PxFilterFlags PhysicsUtils::FilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

	if (filterData0.word3 == (uint)CollisionDetectionType::Continuous || filterData1.word3 == (uint)CollisionDetectionType::Continuous)
	{
		pairFlags |= physx::PxPairFlag::eDETECT_DISCRETE_CONTACT;
		pairFlags |= physx::PxPairFlag::eDETECT_CCD_CONTACT;
	}

	if ((filterData0.word0 & filterData1.word1) || (filterData1.word0 & filterData0.word1))
	{
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
		return physx::PxFilterFlag::eDEFAULT;
	}

	if ((filterData0.word0 & filterData1.word2) || (filterData1.word0 & filterData0.word2))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
		return physx::PxFilterFlag::eDEFAULT;
	}

	return physx::PxFilterFlag::eSUPPRESS;
}
