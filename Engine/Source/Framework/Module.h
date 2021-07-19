#pragma once
#include "Core/Core.h"
#include "ModuleManager.h"

#define EXTERN extern "C"
#define MODULEDEF(name) Module* CreateModule() { return new name(#name); };
#define DEPENDENCY(name) m_Dependencies.push_back({#name});

struct DENGINE_API Dependency
{
	std::string ModuleName;
};

class Application;
class DENGINE_API Module
{
public:
	Module(const std::string& name) : m_Name(name)
	{

	}

	~Module()
	{
		 FreeLibrary(m_Instance);
	}

	virtual void OnLoad()
	{

	}

	virtual void OnUpdate(float DeltaTime)
	{
		
	}

	virtual void OnUnload()
	{

	}

	Ref<Application> GetApplication()
	{
		return m_App;
	}

	template<typename T>
	T* GetModule()
	{
		for (auto& mod : m_ModuleManager->m_LoadedModules)
		{
			T* FoundModule = dynamic_cast<T*>(mod);
			if (FoundModule)
			{
				return FoundModule;
			}
		}

		std::cout << "Couldn't get module!" << std::endl;
		return nullptr;
	}

	std::string m_Name;
	HINSTANCE m_Instance;
	std::vector<Dependency> m_Dependencies;
	std::function<void(void)> m_InvokeFunc;
	ModuleManager* m_ModuleManager;
	Ref<Application> m_App;
};