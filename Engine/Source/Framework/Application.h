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

	/*Creates an appobject with an assigned pointer to this app and a random id, then adds it to app object list here
		WARNING : does not work when called from a module, the created app object must also be associated with a module before being created
	*/
	template<class T>
	Ref<T> CreateAppObject(ObjectInitializer& initializer = ObjectInitializer())
	{
		bool valid = std::is_base_of<AppObject, T>::value;
		ASSERT(valid);

		Ref<T> ptr = MakeRef<T>(this);
	
		Ref<AppObject> obj = Cast<AppObject>(ptr);
		m_AppObjects.push_back(obj);

		//Must call this
		initializer.Flags |= ContructFlags::RANDOMID;
		ptr->Initialize(initializer);
		
		return ptr;
	}

	//removed object from appobject array
	void DestroyAppObject(Ref<AppObject> obj)
	{
		m_AppObjects.erase(std::find(m_AppObjects.begin(), m_AppObjects.end(), obj));
	}

	void DestroyAppObject(AppObject* obj)
	{
// 		auto end = std::remove_if(m_AppObjects.begin(), m_AppObjects.end(), [&](Ref<AppObject> appobj)
// 		{
// 			return obj == appobj.get();
// 		});

		auto remove = m_AppObjects.end();
		for (auto it = m_AppObjects.begin(); it != m_AppObjects.end(); it++)
		{
			if (obj == (*it).get())
			{
				remove = it;
				break;
			}
		}

		if (remove != m_AppObjects.end())
			m_AppObjects.erase(remove);
	}

	//Will call initialize to an already existing object 
	void AddAppObject(AppObject* obj)
	{
		ASSERT(obj);

		//assign application and add to object array
		obj->m_Application = (this);
		Ref<AppObject> objref = ToRef<AppObject>(obj);
		m_AppObjects.push_back(objref);

		//Must call
		obj->Initialize(ObjectInitializer(ContructFlags::RANDOMID));	
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

