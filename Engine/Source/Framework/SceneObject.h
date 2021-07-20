#pragma once
#include "ObjectBase.h"
#include "Event/Callback.h"
#include "Event/ModuleEvent.h"
#include "Event/EventDispatcher.h"

class Scene;
/*
* An Object existing in the scope of a Scene, can contain components
* Must call Initialize(const ObjectInitializer& initializer) mannually after constructing
*
*
*/
class DENGINE_API SceneObject : public ObjectBase
{
public:
	friend class Scene;

	OBJECT_CLASS_DEF(SceneObject, ObjectBase)

	//override constructor receiving a reference to the scene
	SceneObject(Scene* scene) : ObjectBase(), m_Scene(scene)
	{

	}

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;

	Scene* GetScene() const
	{
		ASSERT(m_Scene);
		return m_Scene;
	}

private:
	Scene* m_Scene = nullptr;
	Callback<ModuleEvent> m_ModuleCallback;
};