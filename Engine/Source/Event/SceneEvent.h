#pragma once
#include "Event/Event.h"


enum class SceneObjectClass
{
	SCENEOBJECT = 0,
	OBJECTCOMPONENT,
	SCENE
};

enum class SceneEventType
{
	OBJECT_PRE_INITIALIZE = 0,
	OBJECT_POST_INITIALIZE,
	OBJECT_PRE_DELETE,
	OBJECT_POST_DELETE,
	COMPONENT_PRE_DELETE,
	SCENE_ONCONSTRUCT
};

class SceneObject;
class Scene;
class ObjectComponent;
struct  SceneEvent : public Event
{
	SceneEventType m_EventType = SceneEventType::OBJECT_PRE_INITIALIZE;
	SceneObjectClass m_ObjectClass = SceneObjectClass::SCENEOBJECT;

	SceneObject* m_SceneObject = nullptr;
	Scene* m_Scene = nullptr;
	ObjectComponent* m_Component = nullptr;

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

	const ObjectComponent* GetComponent() const
	{
		return m_Component;
	}

	Scene* GetScene() const
	{
		return m_Scene;
	}
}; 
