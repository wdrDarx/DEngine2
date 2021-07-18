#pragma once
#include "Core/Core.h"
#include "Framework/Tick.h"
#include "Rendering/Window.h"
#include "Framework/AppObject.h"

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

	//Creates an appobject with an assigned pointer to this app and a random id, then adds it to app object list here
	template<class T, typename ... Args>
	void CreateAppObject(Args&&... args)
	{
		bool valid = std::is_base_of<AppObject, T>::value;
		ASSERT(valid);

		T* ptr = new T(ObjectInitializer(ContructFlags::RANDOMID), ToRef<Application>(this), std::forward<Args>(args)...);
		Ref<AppObject> obj = ToRef<AppObject>(dynamic_cast<AppObject*>(ptr));
		m_AppObjects.push_back(obj);
	}

	const std::vector<Ref<AppObject>> GetAppObjects() const
	{
		return m_AppObjects;
	}
	
protected:
	void MakeWindow(const std::string& name, int width, int height, bool vsync);
	void CoreUpdate(float DeltaTime);
	void Shutdown();

	AppState m_AppState;
	Ref<Window> m_Window;
	std::vector<Ref<AppObject>> m_AppObjects;
};

