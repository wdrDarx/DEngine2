#pragma once
#include "Core/Core.h"
#include <PhysX/PxPhysicsAPI.h>

class PhysXAPI;
class DENGINE_API PhysXDebugger
{
public:
	PhysXDebugger(PhysXAPI* api);
	~PhysXDebugger();

	void StartDebugging(const std::string& filepath, bool networkDebugging = false);
	bool IsDebugging();
	void StopDebugging();

	physx::PxPvd* GetDebugger();

	physx::PxPvd* m_Debugger = nullptr;
	physx::PxPvdTransport* m_Transport = nullptr;
};

