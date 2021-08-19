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
}

void Application::MakeWindow(const std::string& name, int width, int height, bool vsync)
{
	m_Window = MakeRef<Window>(name, width, height);

	if(vsync)
		m_Window->SetVsync(true);

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

	OnUpdate(tick);

	//dispatch on end object update event (slow, but clean solution with events)
	ApplicationEvent updateEndEvent;
	updateEndEvent.m_EventType = ApplicationEventType::OBJECTFINSIHUPDATE;
	//GetEventDispatcher().Dispatch(updateEndEvent);
}

void Application::RegisterBaseClasses()
{
 	REGISTER_OBJECT(GetObjectRegistry(), ObjectBase, Engine);
 	REGISTER_OBJECT(GetObjectRegistry(), AppObject, Engine);
 	REGISTER_OBJECT(GetObjectRegistry(), Scene, Engine);
	REGISTER_OBJECT(GetObjectRegistry(), SceneObject, Engine);

	REGISTER_OBJECT(GetObjectRegistry(), Material, Engine);
	REGISTER_OBJECT(GetObjectRegistry(), DefaultMaterial, Engine);
}

void Application::RegisterBaseAssetTypes()
{
	REGISTER_ASSETCLASS(GetAssetManager().GetAssetTypeRegistry(), TextureAsset);
	REGISTER_ASSETCLASS(GetAssetManager().GetAssetTypeRegistry(), PrefabAsset);
	REGISTER_ASSETCLASS(GetAssetManager().GetAssetTypeRegistry(), SceneAsset);
	REGISTER_ASSETCLASS(GetAssetManager().GetAssetTypeRegistry(), ObjectAsset);
	REGISTER_ASSETCLASS(GetAssetManager().GetAssetTypeRegistry(), MaterialAsset);
	REGISTER_ASSETCLASS(GetAssetManager().GetAssetTypeRegistry(), MeshAsset);
}

void Application::Shutdown()
{
	
}
