#pragma once
#include "Event/Event.h"


enum class SceneObjectClass
{
	SCENEOBJECT = 0,
	OBJECTCOMPONENT
};

enum class SceneEventType
{
	OBJECT_PRE_INITIALIZE = 0,
	OBJECT_POST_INITIALIZE,
	OBJECT_PRE_DELETE,
	OBJECT_POST_DELETE,
	SCENE_ONCONSTRUCT
};

class SceneObject;
class Scene;
struct  SceneEvent : public Event
{
	SceneEventType m_EventType;
	SceneObjectClass m_ObjectClass;

	SceneObject* m_SceneObject = nullptr;
	Scene* m_Scene = nullptr;

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

	Scene* GetScene() const
	{
		return m_Scene;
	}
}; 
