#pragma once
#include "Application.h"
#include "DEngine.h"

void Application::MakeWindow(const std::string& name, int width, int height, bool vsync)
{
	m_Window = MakeRef<Window>(name, width, height);

	if(vsync)
		m_Window->SetVsync(true);
}

void Application::CoreUpdate(float DeltaTime)
{
	//NOTE: window start and end frame needs to be called manually because an app can have no window
	OnUpdate(Tick(GetAppState() == AppState::EDITOR ? TickGroup::EDITOR : TickGroup::GAME, DeltaTime));
}

void Application::Shutdown()
{

}
