#pragma once
#include "AppObject.h"
#include "SceneObject.h"
#include "Event/Callback.h"
#include "Event/SceneEvent.h"
#include "Event/EventDispatcher.h"

/*
* An AppObject that containts scoped SceneObjects and relays functions to them
*
* Must call Initialize(const ObjectInitializer& initializer) mannually after constructing
*
*
*/
class DENGINE_API Scene : public AppObject
{
public:
	OBJECT_CLASS_DEF(Scene, AppObject)

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;

	const std::vector<Ref<SceneObject>>& GetSceneObjects() const
	{
		return m_SceneObjects;
	}

	/*  Creates an SceneObject with an assigned pointer to this scene and a random id, then adds it to scene object list here
		WARNING : does not work when called from a module, the created app object must also be associated with a module before being created
	*/
	template<class T>
	Ref<T> CreateSceneObject(ObjectInitializer& initializer = ObjectInitializer())
	{
		bool valid = std::is_base_of<SceneObject, T>::value;
		ASSERT(valid);

		Ref<T> ptr = MakeRef<T>(this);

		Ref<SceneObject> obj = Cast<SceneObject>(ptr);
		m_SceneObjects.push_back(obj);

		//Must call this
		initializer.Flags |= ContructFlags::RANDOMID;
		ptr->Initialize(initializer);

		return ptr;
	}

	//remove object from Scene Object array
	void DestroySceneObject(Ref<SceneObject> obj);

	//remove object from Scene Object array
	void DestroySceneObject(SceneObject* obj);

	//Will call initialize to an already existing scene object 
	void AddSceneObject(SceneObject* obj);

	EventDispatcher& GetSceneEventDipatcher() 
	{
		return m_EventDispatcher;
	}

private:

	//for scene events
	EventDispatcher m_EventDispatcher;

	std::vector<Ref<SceneObject>> m_SceneObjects;
};