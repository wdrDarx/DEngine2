#pragma once
#include "Core/Core.h"
#include "Framework/Tick.h"
#include "Rendering/Window.h"

enum class AppState
{
	GAME = 1,
	EDITOR = 2
};


/*
Application contains a window and houses application objects
*/
class Engine;
class DENGINE_API Application
{
public:
	friend class Engine;

	Application()
	{

	}

	virtual void OnBeginPlay()
	{

	}

	virtual void OnUpdate(const Tick& tick)
	{
		
	}

	const AppState& GetAppState() const
	{		
		return m_AppState;
	}

	void SetAppState(const AppState& state)
	{
		m_AppState = state;
	}

	Ref<Window> GetWindow() const
	{
		ASSERT(m_Window);
		return m_Window;
	}
	
protected:
	void MakeWindow(const std::string& name, int width, int height, bool vsync);
	void CoreUpdate(float DeltaTime);
	void Shutdown();

	AppState m_AppState;
	Ref<Window> m_Window;
};

