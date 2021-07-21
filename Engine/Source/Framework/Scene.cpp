#include "Scene.h"

void Scene::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	//update scene objects
	for (uint i = 0; i < GetSceneObjects().size(); i++)
	{
		auto obj = GetSceneObjects()[i];

		if(CastCheck<SceneObject>(obj))
			obj->OnUpdate(tick);
		else
		{
			LogTemp("bruh");
		}
	}
}

void Scene::OnConstruct()
{
	Super::OnConstruct();

}

void Scene::DestroySceneObject(SceneObject* obj)
{
	auto remove = m_SceneObjects.end();
	for (auto it = m_SceneObjects.begin(); it != m_SceneObjects.end(); it++)
	{
		if (obj == (*it).get())
		{
			remove = it;
			break;
		}
	}

	if (remove != m_SceneObjects.end())
	{
		SceneEvent event;
		event.m_EventType = SceneEventType::PRE_DELETE;
		event.m_ObjectClass = SceneObjectClass::SCENEOBJECT;
		event.m_SceneObject = obj;

		//Dispatch PRE_DELETE event
		GetSceneEventDipatcher().Dispatch(event);
		m_SceneObjects.erase(remove);

		//Dispatch POST_DELETE event
  		event.m_EventType = SceneEventType::POST_DELETE;
		GetSceneEventDipatcher().Dispatch(event);
	}
}

void Scene::DestroySceneObject(Ref<SceneObject> obj)
{
	SceneEvent event;
	event.m_EventType = SceneEventType::PRE_DELETE;
	event.m_ObjectClass = SceneObjectClass::SCENEOBJECT;
	event.m_SceneObject = obj.get();

	//Dispatch PRE_DELETE event
	GetSceneEventDipatcher().Dispatch(event);

	m_SceneObjects.erase(std::find(m_SceneObjects.begin(), m_SceneObjects.end(), obj));

	//Dispatch POST_DELETE event
	event.m_EventType = SceneEventType::POST_DELETE;
	GetSceneEventDipatcher().Dispatch(event);
}

void Scene::AddSceneObject(SceneObject* obj)
{
	ASSERT(obj);

	SceneEvent event;
	event.m_EventType = SceneEventType::PRE_INITIALIZE;
	event.m_ObjectClass = SceneObjectClass::SCENEOBJECT;
	event.m_SceneObject = obj;
	//Dispatch PRE_INITIALIZE event
	GetSceneEventDipatcher().Dispatch(event);

	//assign application and add to object array
	obj->m_Scene = (this);
	Ref<SceneObject> objref = ToRef<SceneObject>(obj);
	m_SceneObjects.push_back(objref);

	//Must call
	obj->Initialize(ObjectInitializer(ContructFlags::RANDOMID));

	//Dispatch POST_INITIALIZE event
	event.m_EventType = SceneEventType::POST_INITIALIZE;
	GetSceneEventDipatcher().Dispatch(event);
}
