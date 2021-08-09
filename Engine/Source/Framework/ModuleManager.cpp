#include "ModuleManager.h"
#include "Module.h"
#include "Framework/Application.h"
#include "Event/ModuleEvent.h"

void ModuleManager::LoadModule(const std::string& FullPath)
{
	//load the dll
	auto TempInstance = LoadLibraryA(FullPath.c_str());
	auto func = (CreateModuleFunc)GetProcAddress(TempInstance, "CreateModule");
	Module* mod = func(); //get the custom class module instance
	mod->m_Instance = TempInstance; //assign the module its instance

	//check for dependencies before loading (Module exists in memory at this point)
	bool ReadyToLoad = true;
	for (auto& dep : mod->m_Dependencies)
	{
		bool found = false;
		for (auto& loadedMod : m_LoadedModules)
		{
			if(loadedMod->m_Name == dep.ModuleName)
			{ 
				found = true;
				break;
			}
		}
		if (found)
		{
			continue;
		}
		else
		{
			//missing a dependency
			ReadyToLoad = false;
		}

	}

	//ready to load
	if(ReadyToLoad)
	{ 
		m_LoadedModules.push_back(mod); //add to module array
		mod->m_ModuleManager = this;
		mod->m_App = m_App;
		m_App->GetEventDispatcher().Dispatch(ModuleEvent(ModuleEventType::LOADED, mod->m_Name));
		mod->OnLoad();
	}
	else //put on a waiting list
	{
		m_PendingLoad.push_back(mod);
	}

	CheckPendingModules();
}

void ModuleManager::CheckPendingModules()
{
	for (auto it = m_PendingLoad.begin(); it != m_PendingLoad.end();)
	{
		Module* mod = *it;
		bool ReadyToLoad = true;

		//check if all dependencies are satisfied
		for (auto& dep : mod->m_Dependencies)
		{
			bool depFound = false;
			for (auto& loaded : m_LoadedModules)
			{
				if (loaded->m_Name == dep.ModuleName)
				{
					depFound = true;
					break;
				}
			}
			if(!depFound)
			{ 
				//if a dependency is missing mark as not ready to load and forget the rest 
				ReadyToLoad = false;
				break;
			}
			//otherwise keep checking
		}

		//all dependencies are satisfied
		if (ReadyToLoad)
		{
			//move the module from pending to loaded
			m_PendingLoad.erase(it);
			m_LoadedModules.push_back(mod);
			mod->m_ModuleManager = this;
			mod->m_App = m_App;
			m_App->GetEventDispatcher().Dispatch(ModuleEvent(ModuleEventType::LOADED, mod->m_Name));
			mod->OnLoad();

			it = m_PendingLoad.begin();
		}
		else
		{		
			//increment itterator to continue
			it++;
		}
	}
}

bool ModuleManager::isModuleLoaded(const std::string& ModuleName)
{
	for (auto it = m_LoadedModules.begin(); it != m_LoadedModules.end(); it++)
	{
		Module* mod = *it;
		if (mod->m_Name == ModuleName)
		{
			return true;
		}
	}
}

void ModuleManager::HotReloadModule(const std::string& ModuleName)
{
	for (auto it = m_LoadedModules.begin(); it != m_LoadedModules.end(); it++)
	{
		Module* mod = *it;
		if (mod->m_Name == ModuleName)
		{

		}
	}
}

void ModuleManager::LoadAllModules(const std::string& FolderPath)
{
	for (const auto& file : std::filesystem::recursive_directory_iterator(FolderPath))
	{
		if (file.is_regular_file() && file.path().extension() == ".dll")
		{
			LoadModule(file.path().string());
		}
	}
}



void ModuleManager::UnloadModule(const std::string& ModuleName)
{
	for (auto it = m_LoadedModules.begin(); it != m_LoadedModules.end(); it++)
	{
		Module* mod = *it;
		if (mod->m_Name == ModuleName)
		{			
			m_App->GetEventDispatcher().Dispatch(ModuleEvent(ModuleEventType::UNLOADED, mod->m_Name));
			mod->OnUnload();
			delete mod;
			m_LoadedModules.erase(it);
			break;
		}
	}
}

void ModuleManager::UnloadAllModules()
{
	for (size_t i = 0; i < m_LoadedModules.size(); i++)
	{
		Module* mod = m_LoadedModules[i];
		m_App->GetEventDispatcher().Dispatch(ModuleEvent(ModuleEventType::UNLOADED, mod->m_Name));
		mod->OnUnload();
		delete mod;
	}
	m_LoadedModules.clear();
}

void ModuleManager::Update(float DeltaTime)
{
	for (auto it = m_LoadedModules.begin(); it != m_LoadedModules.end(); it++)
	{
		(*it)->OnUpdate(DeltaTime);
	}
}
