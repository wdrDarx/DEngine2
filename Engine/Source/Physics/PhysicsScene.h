#pragma once
#include "Core/Core.h"
#include "PhysicsSettings.h"
#include "PhysicsActor.h"
#include "ContactListener.h"
#include <PhysX/PxPhysicsAPI.h>

#define OVERLAP_MAX_COLLIDERS 10

struct RaycastHit
{
// 	vec3d Position;
// 	vec3d Normal;
// 	float Distance;
};

class PhysicsWorld;
class DENGINE_API PhysicsScene 
{
public:
	PhysicsScene(PhysicsWorld* world, const PhysicsSettings& settings);
	~PhysicsScene();

	void Simulate(float ts, bool callFixedUpdate = true);

	//call after Creating the physics actor, and calling CreateRigidActor()
	void AddActor(Ref<PhysicsActor> actor);
	void RemoveActor(Ref<PhysicsActor> actor);
	void RemoveActor(PhysicsActor* actor);

	vec3d GetGravity() const
	{
		return PhysicsUtils::FromPhysXVector(m_PhysXScene->getGravity()); 
	}

	void SetGravity(const vec3d& gravity)
	{ 
		m_PhysXScene->setGravity(PhysicsUtils::ToPhysXVector(gravity));
	}

	bool Raycast(const vec3d& Start, const vec3d& End, RaycastHit* outHit);
	bool OverlapBox(const vec3d& origin, const vec3d& halfSize, std::array<physx::PxOverlapHit, OVERLAP_MAX_COLLIDERS>& buffer, uint& count);

	bool IsValid() const
	{ 
		return m_PhysXScene != nullptr; 
	}

	PhysicsWorld* GetPhysicsWorld()
	{
		return m_PhysicsWorld;
	}

	physx::PxScene* GetPhysXScene()
	{
		return m_PhysXScene;
	}


private:
	void CreateRegions();

	bool Advance(float ts);
	void SubstepStrategy(float ts, uint32_t& substepCount, float& substepSize);
	void Destroy();
	bool OverlapGeometry(const vec3d& origin, const physx::PxGeometry& geometry, std::array<physx::PxOverlapHit, OVERLAP_MAX_COLLIDERS>& buffer, uint& count);

private:
	physx::PxScene* m_PhysXScene;
	std::vector<Ref<PhysicsActor>> m_Actors;

	float m_SubStepSize;
	float m_Accumulator = 0.0f;
	uint m_NumSubSteps = 0;
	const uint c_MaxSubSteps = 1000;

	PhysicsWorld* m_PhysicsWorld;
	ContactListener m_ContactListener;
private:
	friend class PhysicsWorld;
};
