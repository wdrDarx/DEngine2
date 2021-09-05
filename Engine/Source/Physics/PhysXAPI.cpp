#include "DEngine.h"
#include "PhysXAPI.h"
#include "CookingFactory.h"
#include "PhysXDebugger.h"
#include "PhysX/PxFoundation.h"

void PhysicsErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
{
	const char* errorMessage = NULL;

	switch (code)
	{
	case physx::PxErrorCode::eNO_ERROR:				errorMessage = "No Error"; break;
	case physx::PxErrorCode::eDEBUG_INFO:			errorMessage = "Info"; break;
	case physx::PxErrorCode::eDEBUG_WARNING:		errorMessage = "Warning"; break;
	case physx::PxErrorCode::eINVALID_PARAMETER:	errorMessage = "Invalid Parameter"; break;
	case physx::PxErrorCode::eINVALID_OPERATION:	errorMessage = "Invalid Operation"; break;
	case physx::PxErrorCode::eOUT_OF_MEMORY:		errorMessage = "Out Of Memory"; break;
	case physx::PxErrorCode::eINTERNAL_ERROR:		errorMessage = "Internal Error"; break;
	case physx::PxErrorCode::eABORT:				errorMessage = "Abort"; break;
	case physx::PxErrorCode::ePERF_WARNING:			errorMessage = "Performance Warning"; break;
	case physx::PxErrorCode::eMASK_ALL:				errorMessage = "Unknown Error"; break;
	}

	switch (code)
	{
	case physx::PxErrorCode::eNO_ERROR:
	case physx::PxErrorCode::eDEBUG_INFO:
	case physx::PxErrorCode::eDEBUG_WARNING:
	case physx::PxErrorCode::ePERF_WARNING:
		LogWarning("[PhysX] " + std::string(errorMessage) + " " + std::string(message));
		break;
	case physx::PxErrorCode::eINVALID_PARAMETER:
	case physx::PxErrorCode::eINVALID_OPERATION:
	case physx::PxErrorCode::eOUT_OF_MEMORY:
	case physx::PxErrorCode::eINTERNAL_ERROR:
		LogError("[PhysX] " + std::string(errorMessage) + " " + std::string(message));
		break;
	case physx::PxErrorCode::eABORT:
	case physx::PxErrorCode::eMASK_ALL:
		LogError("[PhysX] " + std::string(errorMessage) + " " + std::string(message));
		ASSERT(false);
		break;
	}
}

void PhysicsAssertHandler::operator()(const char* exp, const char* file, int line, bool& ignore)
{
	LogError("[PhysX] " + std::string(exp) + " " + std::string(file));
}

PhysXAPI::PhysXAPI()
{
	m_PhysXFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
	ASSERT(m_PhysXFoundation);
	
	//default sim values (1 cm = 1.f)
	m_PhysicsWorldScale.length = 100.0f;
	m_PhysicsWorldScale.speed = 981.f;

	m_Debugger = MakeRef<PhysXDebugger>(this);
	m_Debugger->StartDebugging("", true);
	m_PhysXSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PhysXFoundation, m_PhysicsWorldScale, false, m_Debugger->GetDebugger());
	ASSERT(m_PhysXSDK);

	s_ExtensionsLoaded = true;
	m_PhysXCPUDispatcher = physx::PxDefaultCpuDispatcherCreate(std::thread::hardware_concurrency());

	// create GPU dispatcher
	physx::PxCudaContextManagerDesc cudaContextManagerDesc;
	m_PhysXCudaContextManager = PxCreateCudaContextManager(*m_PhysXFoundation, cudaContextManagerDesc);

	//CookingFactory::Initialize();

	PxSetAssertHandler(m_AssertHandler);
}

PhysXAPI::~PhysXAPI()
{
	//CookingFactory::Shutdown();

	m_PhysXCPUDispatcher->release();
	m_PhysXCPUDispatcher = nullptr;
	m_RigidBodyExt = nullptr;

	m_Debugger->StopDebugging();
	Unload();

	m_PhysXSDK->release();
	m_PhysXSDK = nullptr;


	m_PhysXFoundation->release();
	m_PhysXFoundation = nullptr;
}

bool PhysXAPI::setMassAndUpdateInertia(physx::PxRigidBody& body, physx::PxReal mass, const physx::PxVec3* massLocalPose /*= NULL*/, bool includeNonSimShapes /*= false*/)
{
	Load();
	return physx::PxRigidBodyExt::setMassAndUpdateInertia(body, mass, massLocalPose, includeNonSimShapes);
}

physx::PxFoundation* PhysXAPI::GetFoundation() { return m_PhysXFoundation; }
physx::PxPhysics* PhysXAPI::GetPhysXSDK() { return m_PhysXSDK; }
physx::PxCpuDispatcher* PhysXAPI::GetCPUDispatcher() { return m_PhysXCPUDispatcher; }
physx::PxCudaContextManager* PhysXAPI::GetCudaContextManager() { return m_PhysXCudaContextManager;}

physx::PxDefaultAllocator* PhysXAPI::GetAllocator() { return &m_Allocator; }

void PhysXAPI::Load()
{
	if(!s_ExtensionsLoaded)
	{ 
		auto foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
		auto sdk = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, m_PhysicsWorldScale, false, m_Debugger->GetDebugger());
		bool extentionsLoaded = PxInitExtensions(*sdk, GetDebugger()->GetDebugger());

		ASSERT(extentionsLoaded);
		s_ExtensionsLoaded = true;
	}
}

void PhysXAPI::Unload()
{
	if(s_ExtensionsLoaded)
	{ 
		PxCloseExtensions();
		//debugger should be deleted here fyi
		s_ExtensionsLoaded = false;
	}
}

bool PhysXAPI::s_ExtensionsLoaded = false;
