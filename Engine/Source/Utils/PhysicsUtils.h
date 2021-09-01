#pragma once
#include "Core/Core.h"
#include "Physics/PhysicsSettings.h"
#include <PhysX/PxPhysicsAPI.h>

enum class CookingResult
{
	Success,
	ZeroAreaTestFailed,
	PolygonLimitReached,
	LargeTriangle,
	Failure
};

enum class ForceMode : uint8_t
{
	Force = 0,
	Impulse,
	VelocityChange,
	Acceleration
};

enum class ActorLockFlag
{
	PositionX = BIT(0), PositionY = BIT(1), PositionZ = BIT(2),
	RotationX = BIT(3), RotationY = BIT(4), RotationZ = BIT(5)
};

class DENGINE_API PhysicsUtils
{
	public:
		static physx::PxTransform ToPhysXTransform(const Transform& transform);
		static physx::PxTransform ToPhysXTransform(const glm::mat4& transform);
		static physx::PxTransform ToPhysXTransform(const vec3d& translation, const vec3d& rotation);
		static physx::PxMat44 ToPhysXMatrix(const glm::mat4& matrix);
		static const physx::PxVec3& ToPhysXVector(const vec3d& vector);
		static const physx::PxVec4& ToPhysXVector(const vec4d& vector);
		static physx::PxQuat ToPhysXQuat(const glm::quat& q);

		static glm::mat4 FromPhysXTransform(const physx::PxTransform& transform);
		static glm::mat4 FromPhysXMatrix(const physx::PxMat44& matrix);
		static vec3d FromPhysXVector(const physx::PxVec3& vector);
		static vec4d FromPhysXVector(const physx::PxVec4& vector);
		static glm::quat FromPhysXQuat(const physx::PxQuat& quat);

		static CookingResult FromPhysXCookingResult(physx::PxConvexMeshCookingResult::Enum cookingResult);
		static CookingResult FromPhysXCookingResult(physx::PxTriangleMeshCookingResult::Enum cookingResult);

		static physx::PxBroadPhaseType::Enum ToPhysXBroadphaseType(BroadphaseType type);
		static physx::PxFrictionType::Enum ToPhysXFrictionType(FrictionType type);

		static physx::PxRigidDynamicLockFlag::Enum ToPhysXActorLockFlag(ActorLockFlag flag);

		static physx::PxFilterFlags FilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1,
			physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);
};

