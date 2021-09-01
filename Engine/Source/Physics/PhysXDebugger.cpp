#include "Core/Core.h"
#include "PhysXDebugger.h"
#include "PhysXAPI.h"

PhysXDebugger::PhysXDebugger(PhysXAPI* api)
{
	m_Debugger = PxCreatePvd(*api->GetFoundation());
	ASSERT(m_Debugger);
}

PhysXDebugger::~PhysXDebugger()
{
	ASSERT(m_Debugger);
	m_Debugger->release();
	m_Debugger = nullptr;
}

void PhysXDebugger::StartDebugging(const std::string& filepath, bool networkDebugging /*= false*/)
{
	StopDebugging();

	if (!networkDebugging)
	{
		m_Transport = physx::PxDefaultPvdFileTransportCreate((filepath + ".pxd2").c_str());
		GetDebugger()->connect(*m_Transport, physx::PxPvdInstrumentationFlag::eALL);
	}
	else
	{
		m_Transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10000);
		GetDebugger()->connect(*m_Transport, physx::PxPvdInstrumentationFlag::eALL);
	}
}

bool PhysXDebugger::IsDebugging()
{
	return GetDebugger()->isConnected();
}

void PhysXDebugger::StopDebugging()
{
	if (!m_Debugger->isConnected())
		return;

	m_Debugger->disconnect();
	m_Transport->release();
}

physx::PxPvd* PhysXDebugger::GetDebugger()
{
	return m_Debugger;
}

