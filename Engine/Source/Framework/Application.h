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
#include "Event/ModuleEvent.h"

//assets
#include "Assets/AssetManager.h"

enum class AppState
{
	GAME = 1,
	EDITOR = 2
};

enum class AppType
{
	DEVELOPMENT = 1,
	SHIPPING = 2
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

	virtual void OnUpdate(const Tick& tick);

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
		initializer.Flags |= ConstructFlags::RANDOMID;
		ptr->Initialize(initializer);
		
		return ptr;
	}

	template<class T>
	std::vector<Ref<T>> FindObjectsByClass()
	{
		bool valid = std::is_base_of<AppObject, T>::value;
		ASSERT(valid);

		std::vector<Ref<T>> out;
		for (auto& obj : m_AppObjects)
		{
			if(Ref<T> outObj = Cast<T>(obj))
				out.push_back(outObj);
		}

		return out;
	}

	//removed object from appobject array
	void DestroyAppObject(Ref<AppObject> obj)
	{
		//call on destroy
		obj->OnDestroy();
		m_AppObjects.erase(std::find(m_AppObjects.begin(), m_AppObjects.end(), obj));
	}

	void DestroyAppObject(AppObject* obj)
	{
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
		{ 
			//call on destroy
			(*remove)->OnDestroy();
			m_AppObjects.erase(remove);
		}
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
		obj->Initialize(ObjectInitializer(ConstructFlags::RANDOMID));	
	}

	template<class T>
	Ref<T> GetAppObject()
	{
		for (auto& obj : GetAppObjects())
		{
			if(Ref<T> out = Cast<T>(obj))
				return out;
		}

		return nullptr;
	}

	const std::vector<Ref<AppObject>> GetAppObjects() const
	{
		return m_AppObjects;
	}

	ObjectRegistry& GetObjectRegistry()
	{
		return m_ObjectRegistry;
	}

	StructRegistry& GetStructRegistry()
	{
		return m_StructRegistry;
	}

	ModuleManager& GetModuleManager()
	{
		return m_ModuleManager;
	}

	EventDispatcher& GetEventDispatcher()
	{
		return m_EventDispatcher;
	}

	AssetManager& GetAssetManager()
	{
		return m_AssetManager;
	}

	AppType GetAppType() const
	{
		return m_AppType;
	}

	void SetAppType(AppType type)
	{
		m_AppType = type;
	}
	
protected:
	void MakeWindow(const std::string& name, int width, int height, bool vsync);
	void CoreUpdate(float DeltaTime);
	void RegisterBaseClasses();
	void RegisterBaseAssetTypes();
	void Shutdown();

	AppState m_AppState;
	AppType m_AppType;
	Ref<Window> m_Window;
	
	//used to relay window events to this event dispatcher
	Callback<WindowEvent> m_WindowEventCallback;

	//used to auto unregister module classes and auto unmount module dirs
	Callback<ModuleEvent> m_ModuleEventCallback;

	//for all app events (also relays window events)
	EventDispatcher m_EventDispatcher;

	std::vector<Ref<AppObject>> m_AppObjects;

	ObjectRegistry m_ObjectRegistry;
	StructRegistry m_StructRegistry;

	ModuleManager m_ModuleManager;

	AssetManager m_AssetManager;

	Tick m_LastTick;
};

