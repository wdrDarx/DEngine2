#pragma once
#include "Event/Event.h"


enum class SceneObjectClass
{
	SCENEOBJECT = 0,
	OBJECTCOMPONENT
};

enum class SceneEventType
{
	PRE_INITIALIZE = 0,
	POST_INITIALIZE,
	PRE_DELETE,
	POST_DELETE
};

class SceneObject;
struct DENGINE_API SceneEvent : public Event
{
	SceneEventType m_EventType;
	SceneObjectClass m_ObjectClass;

	SceneObject* m_SceneObject;

	SceneEvent()
	{

	}

	const SceneEventType& GetEventType() const
	{
		return m_EventType;
	}

	const SceneObjectClass& GetSceneObjectClass() const
	{
		return m_ObjectClass;
	}

	const SceneObject* GetSceneObject() const
	{
		return m_SceneObject;
	}
}; 
