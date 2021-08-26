#pragma once
#include "ObjectBase.h"
#include "Event/Callback.h"
#include "Event/ModuleEvent.h"
#include "Event/EventDispatcher.h"

class Application;
/*
  An Object existent in the scope of an app with a reference to it
  Must call Initialize(const ObjectInitializer& initializer) mannually after constructing
*/
class DENGINE_API AppObject : public ObjectBase
{
public:
	friend class Application;

	OBJECT_CLASS_DEF(AppObject, ObjectBase)

	//override constructor receiving a reference to the app
	AppObject(Application* app) : ObjectBase(), m_Application(app)
	{
		
	}

	//begin play event
	virtual void OnBeginPlay()
	{

	}

	void OnUpdate(const Tick& tick) override;

	void OnConstruct() override;

	Application* GetApplication() const
	{
		ASSERT(m_Application);
		return m_Application;
	}

	//get the last tick that was set on update
	const Tick& GetLastTick() const
	{
		return m_LastTick;
	}

	void SetLastTick(const Tick& tick)
	{
		m_LastTick = tick;
	}

private:
	Application* m_Application = nullptr;
	Callback<ModuleEvent> m_ModuleCallback;
	Tick m_LastTick;
};