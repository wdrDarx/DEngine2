#pragma once
#include "PhysicsSettings.h"
#include <PhysX/PxPhysicsAPI.h>
#include "PhysX/extensions/PxRigidActorExt.h"
#include "PhysXDebugger.h"

class PhysicsErrorCallback : public physx::PxErrorCallback
{
public:
	 void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
};

class PhysicsAssertHandler : public physx::PxAssertHandler
{
	void operator()(const char* exp, const char* file, int line, bool& ignore);
};


class DENGINE_API PhysXAPI
{
public:
	PhysXAPI();
	virtual ~PhysXAPI();

	Ref<PhysXDebugger> GetDebugger()
	{
		return m_Debugger;
	}

	bool setMassAndUpdateInertia(physx::PxRigidBody& body, physx::PxReal mass, const physx::PxVec3* massLocalPose = NULL, bool includeNonSimShapes = false);

	Ref<physx::PxRigidBodyExt> GetRigidBodyExt()
	{
		return m_RigidBodyExt;
	}

	Ref<physx::PxRigidBodyExt> m_RigidBodyExt;
	Ref<PhysXDebugger> m_Debugger;
	physx::PxFoundation* GetFoundation();
	physx::PxPhysics* GetPhysXSDK();
	physx::PxCpuDispatcher* GetCPUDispatcher();
	physx::PxDefaultAllocator* GetAllocator();

	physx::PxFoundation* m_PhysXFoundation;
	physx::PxDefaultCpuDispatcher* m_PhysXCPUDispatcher;
	physx::PxPhysics* m_PhysXSDK;

	physx::PxDefaultAllocator m_Allocator;
	PhysicsErrorCallback m_ErrorCallback;
	PhysicsAssertHandler m_AssertHandler;

public:
	physx::PxTolerancesScale m_PhysicsWorldScale = physx::PxTolerancesScale();

	void Load();
	void Unload();

	static bool s_ExtensionsLoaded;
};

