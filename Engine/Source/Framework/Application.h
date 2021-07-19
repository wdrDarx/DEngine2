#pragma once
#include "Core/Core.h"

//Framework
#include "Framework/Tick.h"
#include "Framework/AppObject.h"
#include "Framework/ModuleManager.h"
#include "Framework/Registry.h"

//Rendering
#include "Rendering/Window.h"

//events
#include "Event/Callback.h"
#include "Event/WindowEvent.h"

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

	Application();

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

	//Creates an appobject with an assigned pointer to this app and a random id, then adds it to app object list here
	template<class T, typename ... Args>
	void CreateAppObject(Args&&... args)
	{
		bool valid = std::is_base_of<AppObject, T>::value;
		ASSERT(valid);

		T* ptr = new T(ToRef<Application>(this), std::forward<Args>(args)...);

		//Must call this
		ptr->Initialize(ObjectInitializer(ContructFlags::RANDOMID));

		Ref<AppObject> obj = ToRef<AppObject>(dynamic_cast<AppObject*>(ptr));
		m_AppObjects.push_back(obj);
	}

	const std::vector<Ref<AppObject>> GetAppObjects() const
	{
		return m_AppObjects;
	}

	Registry& GetRegistry()
	{
		return m_Registry;
	}

	ModuleManager& GetModuleManager()
	{
		return m_ModuleManager;
	}

	EventDispatcher& GetEventDispatcher()
	{
		return m_EventDispatcher;
	}
	
protected:
	void MakeWindow(const std::string& name, int width, int height, bool vsync);
	void CoreUpdate(float DeltaTime);
	void RegisterBaseClasses();
	void Shutdown();

	AppState m_AppState;
	Ref<Window> m_Window;
	
	//used to relay window events to this event dispatcher
	Callback<WindowEvent> m_WindowEventCallback;

	//for all app events (also relays window events)
	EventDispatcher m_EventDispatcher;

	std::vector<Ref<AppObject>> m_AppObjects;

	Registry m_Registry;
	ModuleManager m_ModuleManager;
};

