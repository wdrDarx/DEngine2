#include "CustomClasses.h"

#include "Classes/CustomAppObject.h"
#include "Classes/StaticMeshObject.h"
#include "Classes/CustomSceneObject.h"
#include "Classes/ChildSceneObject.h"
#include "Classes/QuadComponent.h"
#include "Classes/MeshComponent.h"
#include "Classes/StaticMeshComponent.h"
#include "Classes/CameraComponent.h"
#include "Classes/PlayerController.h"
#include "Classes/Bullet.h"

#include "Classes/QuadRenderer.h"
#include "Classes/MyObject.h"

#include "tlhelp32.h"
#include <comdef.h> 


void GetModuleMemory(DWORD dwProcID, char* szModuleName, uptr& memPtr, uint64& memSize)
{
	uptr ModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(0x00000008 | 0x00000010, dwProcID);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 ModuleEntry32;
		ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &ModuleEntry32))
		{
			do
			{
				if (strcmp(_bstr_t(ModuleEntry32.szModule), szModuleName) == 0)
				{
					memPtr = (uptr)ModuleEntry32.modBaseAddr;
					memSize = (uint64)ModuleEntry32.modBaseSize;
					break;
				}
			} while (Module32Next(hSnapshot, &ModuleEntry32));
		}
		CloseHandle(hSnapshot);
	}
}

void CustomClasses::OnLoad()
{
	//cancer
	Module::AssignApplication(GetApplication());

	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomAppObject, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomSceneObject, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), ChildSceneObject, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), StaticMeshObject, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), QuadComponent, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), MeshComponent, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), StaticMeshComponent, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), QuadMaterial, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CameraComponent, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), PlayerController, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), Bullet, CustomClasses);

	
// 	//custom renderer
// 	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), QuadRenderer, CustomClasses);
// 
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
	}
// 
// 	auto vec = GetApplication()->FindObjectsByClass<Scene>();
// 	if(vec.size() == 0)
// 	{
// 		m_SceneEvent.Assign([&](SceneEvent* event)
// 			{
// 				if (event->GetEventType() == SceneEventType::SCENE_ONCONSTRUCT)
// 				{
// 					if (!gladLoadGL())
// 					{
// 						std::cout << "Failed to initialize OpenGL context" << std::endl;
// 					}
// 
// 					event->GetScene()->CreateRenderer<QuadRenderer>(ObjectInitializer::Module(GetThisModuleName()));
// 				}
// 			});
// 		GetApplication()->GetEventDispatcher().Bind(m_SceneEvent);
// 	}
// 	else
// 	for(const Ref<Scene>& scene : vec)
// 	{
// 		scene->CreateRenderer<QuadRenderer>(ObjectInitializer::Module(GetThisModuleName()));
// 	}
	
	REGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct);
	REGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct2);

	GetApplication()->GetAssetManager().MountContentDirectory(Paths::GetModulesDirectory());
}

void CustomClasses::OnUnload()
{
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomAppObject, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomSceneObject, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), ChildSceneObject, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), StaticMeshObject, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), QuadComponent, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), MeshComponent, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), StaticMeshComponent, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), QuadMaterial, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CameraComponent, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), PlayerController, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), Bullet, CustomClasses);

	//UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), QuadRenderer, CustomClasses);

	UNREGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct);
	UNREGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct2);

	GetApplication()->GetAssetManager().UnmountContentDirectory(Paths::GetModulesDirectory());
}

void CustomClasses::OnUpdate(float DeltaTime)
{
	
}
