#pragma once
#include "ObjectBase.h"
#include "ObjectComponent.h"
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

	//need explicit destructor to call pre_delete events for all components
	~SceneObject();

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;

	Scene* GetScene() const
	{
		ASSERT(m_Scene);
		return m_Scene;
	}

	const std::vector<Ref<ObjectComponent>>& GetComponents() const
	{
		return m_Components;
	}

	/*  
		Creates an ObjectComponent with an assigned pointer to this SceneObject and a random id, then adds it to scene object list here
		WARNING : does not work when called from a module, the created ObjectComponent must also be associated with a module before being created
	*/
	template<class T>
	Ref<T> CreateComponent(ObjectInitializer& initializer = ObjectInitializer())
	{
		bool valid = std::is_base_of<ObjectComponent, T>::value;
		ASSERT(valid);

		Ref<T> ptr = MakeRef<T>(this);

		Ref<ObjectComponent> comp = Cast<ObjectComponent>(ptr);
		m_Components.push_back(comp);

		//Must call this
		initializer.Flags |= ContructFlags::RANDOMID;
		ptr->Initialize(initializer);

		return ptr;
	}

	//remove object from Component array
	void DestroyComponent(Ref<ObjectComponent> comp);

	//remove object from Component array
	void DestroyComponent(ObjectComponent* comp);

	//Will call initialize to an already existing Component
	void AddComponent(ObjectComponent* comp);

private:
	Scene* m_Scene = nullptr;
	Callback<ModuleEvent> m_ModuleCallback;

	//array of owning components
	std::vector<Ref<ObjectComponent>> m_Components;

};