#include "CustomClasses.h"

#include "Classes/CustomAppObject.h"
#include "Classes/SonicGaming.h"
#include "Classes/CustomSceneObject.h"
#include "Classes/ChildSceneObject.h"
#include "Classes/QuadComponent.h"

#include "Classes/QuadRenderer.h"
#include "Classes/TestAsset.h"

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
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomAppObject, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomSceneObject, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), ChildSceneObject, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), SonicGaming, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), QuadComponent, CustomClasses);

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

	REGISTER_ASSETCLASS(GetApplication()->GetAssetManager().GetAssetTypeRegistry(), TestAsset);

	GetApplication()->GetAssetManager().MountContentDirectory(Paths::GetModulesDirectory());
}

void CustomClasses::OnUnload()
{
	m_SceneEvent.Destroy();
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomAppObject, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomSceneObject, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), ChildSceneObject, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), SonicGaming, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), QuadComponent, CustomClasses);

	//UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), QuadRenderer, CustomClasses);

	UNREGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct);
	UNREGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct2);
	UNREGISTER_ASSETCLASS(GetApplication()->GetAssetManager().GetAssetTypeRegistry(), TestAsset);

	GetApplication()->GetAssetManager().UnmountContentDirectory(Paths::GetModulesDirectory());
}
