#pragma once
#include "Application.h"
#include "DEngine.h"


Application::Application() : m_ModuleManager(ToRef<Application>(this)) //initialie module manager
{
	RegisterBaseClasses();
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
	OnUpdate(Tick(GetAppState() == AppState::EDITOR ? TickGroup::EDITOR : TickGroup::GAME, DeltaTime));
}

void Application::RegisterBaseClasses()
{
	REGISTER(GetRegistry(), AppObject, RegistryType::OBJECT);
}

void Application::Shutdown()
{

}
