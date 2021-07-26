#pragma once
#include "Application.h"
#include "DEngine.h"


Application::Application() : m_ModuleManager(ToRef<Application>(this)) //initialize module manager
{
	RegisterBaseClasses();
	GetObjectRegistry().m_App = ToRef<Application>(this);
	GetStructRegistry().m_App = ToRef<Application>(this);
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

	for (uint i = 0; i < m_AppObjects.size(); i++)
	{
		auto obj = m_AppObjects[i];

		//increment the itterator here 
		if(obj)
			obj->OnUpdate(tick);
	}

	//dipatch on end object update event
	ApplicationEvent updateEndEvent;
	updateEndEvent.m_EventType = ApplicationEventType::OBJECTFINSIHUPDATE;
	GetEventDispatcher().Dispatch(updateEndEvent);
}

void Application::RegisterBaseClasses()
{
 	REGISTER_OBJECT(GetObjectRegistry(), ObjectBase, Engine);
 	REGISTER_OBJECT(GetObjectRegistry(), AppObject, Engine);
 	REGISTER_OBJECT(GetObjectRegistry(), Scene, Engine);
	REGISTER_OBJECT(GetObjectRegistry(), SceneObject, Engine);
}

void Application::Shutdown()
{

}
