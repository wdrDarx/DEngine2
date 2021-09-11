#pragma once
#include "Application.h"
#include "DEngine.h"


Application::Application() : m_ModuleManager(ToRef<Application>(this)) //initialize module manager
{
	GetObjectRegistry().m_App = ToRef<Application>(this);
	GetStructRegistry().m_App = ToRef<Application>(this);
	GetAssetManager().GetAssetTypeRegistry().m_App = ToRef<Application>(this);

	RegisterBaseClasses();
	RegisterBaseAssetTypes();

	//mount default content directory
	GetAssetManager().MountContentDirectory(Paths::GetContentDirectory());

	//auto unregister
	m_ModuleEventCallback.Assign([&](ModuleEvent* event)
	{
		if (event->GetEventType() == ModuleEventType::UNLOADED)
		{
			for (auto& reg : GetObjectRegistry().GetRegisteredKeys())
			{
				if (reg.AssignedModuleName == event->m_ModuleName)
				{
					GetObjectRegistry().Unregister(reg);
				}
			}

			for (auto& reg : GetStructRegistry().GetRegisteredKeys())
			{
				if (reg.AssignedModuleName == event->m_ModuleName)
				{
					GetStructRegistry().Unregister(reg);
				}
			}

			for (auto& reg : GetAssetManager().GetAssetTypeRegistry().GetRegisteredKeys())
			{
				if (reg.AssignedModuleName == event->m_ModuleName)
				{
					GetAssetManager().GetAssetTypeRegistry().Unregister(reg);
				}
			}

			for (auto& dir : GetAssetManager().GetMountedDirectories())
			{
				std::string name = std::filesystem::path(dir).parent_path().parent_path().filename().string();
				if (name == event->m_ModuleName)
				{
					GetAssetManager().UnmountContentDirectory(dir);
				}
			}
		}
	});

	//bind to myself
	GetEventDispatcher().Bind(m_ModuleEventCallback);
}

void Application::OnUpdate(const Tick& tick)
{
	//update all the modules
	GetModuleManager().Update(tick.DeltaTime);

	//update all the app objects
	for (uint i = 0; i < m_AppObjects.size(); i++)
	{
		auto obj = m_AppObjects[i];

		if (obj)
			obj->OnUpdate(tick);
	}

	//dispatch object finish update event
	ApplicationEvent event;
	event.m_EventType = ApplicationEventType::OBJECTFINSIHUPDATE;
	GetEventDispatcher().Dispatch(event);
}

void Application::MakeWindow(const std::string& name, int width, int height, bool vsync)
{
	m_Window = MakeRef<Window>(name, width, height);

	m_Window->SetVsync(vsync);

	//assign an onClose event
	m_WindowEventCallback.Assign([&](WindowEvent* event)
	{
		//relay event to app dispatcher
		m_EventDispatcher.Dispatch(*event);

		if (event->GetEventType() == WindowEventType::CLOSED)
		{
			Shutdown();
		}
	});

	//bind the relay callback to the window event dispatcher
	m_Window->GetEventDispatcher().Bind(m_WindowEventCallback);
}

void Application::CoreUpdate(float DeltaTime)
{
	//NOTE: window start and end frame needs to be called manually because an app can have no window
	Tick tick(GetAppState() == AppState::EDITOR ? TickGroup::EDITOR : TickGroup::GAME, DeltaTime);
	m_LastTick = tick;
	OnUpdate(tick);
}

void Application::RegisterBaseClasses()
{
	//Base
 	REGISTER_OBJECT(GetObjectRegistry(), ObjectBase, Engine);
 	REGISTER_OBJECT(GetObjectRegistry(), AppObject, Engine);
 	REGISTER_OBJECT(GetObjectRegistry(), Scene, Engine);
	REGISTER_OBJECT(GetObjectRegistry(), SceneObject, Engine);

	//Material
	REGISTER_OBJECT(GetObjectRegistry(), Material, Engine);
	REGISTER_OBJECT(GetObjectRegistry(), PBR_Material, Engine);

	//Classes
	REGISTER_OBJECT(GetObjectRegistry(), TransformComponent, Engine);
	REGISTER_OBJECT(GetObjectRegistry(), ColliderComponent, Engine);
	REGISTER_OBJECT(GetObjectRegistry(), BoxColliderComponent, Engine);
	REGISTER_OBJECT(GetObjectRegistry(), DirectionalLightObject, Engine);

	//enums
	REGISTER_ENUM(GetStructRegistry(), ProjectionType, Engine);
	REGISTER_ENUM(GetStructRegistry(), CollisionLayerEnum, Engine);
	REGISTER_ENUM(GetStructRegistry(), LayerCollisions, Engine);
}

void Application::RegisterBaseAssetTypes()
{
	REGISTER_ASSETCLASS(GetAssetManager().GetAssetTypeRegistry(), TextureAsset, Engine);
	REGISTER_ASSETCLASS(GetAssetManager().GetAssetTypeRegistry(), PrefabAsset, Engine);
	REGISTER_ASSETCLASS(GetAssetManager().GetAssetTypeRegistry(), SceneAsset, Engine);
	REGISTER_ASSETCLASS(GetAssetManager().GetAssetTypeRegistry(), ObjectAsset, Engine);
	REGISTER_ASSETCLASS(GetAssetManager().GetAssetTypeRegistry(), MaterialAsset, Engine);
	REGISTER_ASSETCLASS(GetAssetManager().GetAssetTypeRegistry(), MeshAsset, Engine);
}

void Application::Shutdown()
{
	
}
