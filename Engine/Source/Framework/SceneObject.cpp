#include "SceneObject.h"
#include "DEngine.h"

SceneObject::~SceneObject()
{
	for (auto& comp : GetComponents())
	{
		DestroyComponent(comp);
	}
}

void SceneObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	//update scene objects
	for (uint i = 0; i < GetComponents().size(); i++)
	{
		auto obj = GetComponents()[i];

		if (CastCheck<ObjectComponent>(obj))
			obj->OnUpdate(tick);
	}
}


void SceneObject::OnConstruct()
{
	Super::OnConstruct();

	//auto destroy object when its module is unloaded
	m_ModuleCallback.Assign([&](ModuleEvent* event)
	{
		if (event->GetEventType() == ModuleEventType::UNLOADED && event->m_ModuleName == GetAssociatedModuleName())
		{
			//unbind auto destruct from all components since its unnecessary now
			for (auto& comp : GetComponents())
			{
				comp->m_ModuleCallback.Destroy();
			}

			GetScene()->DestroySceneObject(this);
		}
	});

	m_Scene->GetApplication()->GetEventDispatcher().Bind(m_ModuleCallback);
}

void SceneObject::DestroyComponent(Ref<ObjectComponent> comp)
{
	//call pre delete event
	SceneEvent PreDeleteEvent;
	PreDeleteEvent.m_Component = comp.get();
	PreDeleteEvent.m_EventType = SceneEventType::COMPONENT_PRE_DELETE;
	PreDeleteEvent.m_ObjectClass = SceneObjectClass::OBJECTCOMPONENT;
	GetScene()->GetSceneEventDipatcher().Dispatch(PreDeleteEvent);

	//prevent component from deleting itself twice
	comp->m_ModuleCallback.Destroy();
	m_Components.erase(std::find(m_Components.begin(), m_Components.end(), comp));
}

void SceneObject::DestroyComponent(ObjectComponent* comp)
{
	auto remove = m_Components.end();
	for (auto it = m_Components.begin(); it != m_Components.end(); it++)
	{
		if (comp == (*it).get())
		{
			remove = it;
			break;
		}
	}

	if (remove != m_Components.end())
	{
		//call pre delete event
		SceneEvent PreDeleteEvent;
		PreDeleteEvent.m_Component = (*remove).get();
		PreDeleteEvent.m_EventType = SceneEventType::COMPONENT_PRE_DELETE;
		PreDeleteEvent.m_ObjectClass = SceneObjectClass::OBJECTCOMPONENT;
		GetScene()->GetSceneEventDipatcher().Dispatch(PreDeleteEvent);

		//prevent component from deleting itself twice
		(*remove)->m_ModuleCallback.Destroy();
		m_Components.erase(remove);
	}
}

void SceneObject::AddComponent(ObjectComponent* comp)
{
	ASSERT(comp);

	//assign owner and add to object array
	comp->m_Owner = (this);
	Ref<ObjectComponent> compref = ToRef<ObjectComponent>(comp);
	m_Components.push_back(compref);

	//Must call
	comp->Initialize(ObjectInitializer(ContructFlags::RANDOMID));
}
