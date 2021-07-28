#pragma once
#include "Core/Core.h"
#include "ModuleManager.h"

#define EXTERN extern "C"
#define MODULEDEF(name) Module* CreateModule() { return new name(#name); };
#define DEPENDENCY(name) m_Dependencies.push_back({#name});

struct Dependency
{
	std::string ModuleName;
};

class Application;
class Module
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

	const std::string GetThisModuleName() const
	{
		return m_Name;
	}

	std::string m_Name;
	HINSTANCE m_Instance = nullptr;
	std::vector<Dependency> m_Dependencies;
	std::function<void(void)> m_InvokeFunc;
	ModuleManager* m_ModuleManager = nullptr;
	Ref<Application> m_App;
};