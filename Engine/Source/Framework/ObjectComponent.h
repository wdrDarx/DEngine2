#pragma once
#include "ObjectBase.h"
#include "Event/Callback.h"
#include "Event/ModuleEvent.h"
#include "Event/EventDispatcher.h"

/*
	inheritable Object existant in the scope of a sceneObject
*/
class SceneObject;
class ObjectComponent : public ObjectBase
{
public:	
	friend class SceneObject;

	OBJECT_CLASS_DEF(ObjectComponent, ObjectBase)

	//override constructor receiving a reference to the SceneObject
	ObjectComponent(SceneObject* obj) : ObjectBase(), m_Owner(obj)
	{
		
	}

	//begin play event
	virtual void OnBeginPlay()
	{

	}

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;

	SceneObject* GetOwner() const
	{
		ASSERT(m_Owner);
		return m_Owner;
	}

private:
	SceneObject* m_Owner = nullptr;
	Callback<ModuleEvent> m_ModuleCallback;
};

