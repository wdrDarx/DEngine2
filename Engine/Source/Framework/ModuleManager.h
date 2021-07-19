#pragma once
#include "Core/Core.h"


class Module;
typedef Module* (*CreateModuleFunc)();

class Application;

class DENGINE_API ModuleManager
{
public:
	ModuleManager(Ref<Application> app) : m_App(app)
	{

	}

	void LoadModule(const std::string& FullPath);
	void LoadAllModules(const std::string& FolderPath);
	void CheckPendingModules();

	void UnloadModule(const std::string& ModuleName);
	void UnloadAllModules();

	void Update(float DeltaTime);

public:
	std::vector<Module*> m_LoadedModules;

	//modules pending to load based on dependencies
	std::vector<Module*> m_PendingLoad;

	//reference to the host app for modules to take on
	Ref<Application> m_App;
};

