#include "Scene.h"
#include "Framework/Application.h"

void Scene::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	//update scene objects
	for (uint i = 0; i < GetSceneObjects().size(); i++)
	{
		auto obj = GetSceneObjects()[i];

		if(CastCheck<SceneObject>(obj))
			obj->OnUpdate(tick);
	}
}

void Scene::OnConstruct()
{
	Super::OnConstruct();
	CreateDefaultRenderers();

	SceneEvent constructEvent;
	constructEvent.m_Scene = this;
	constructEvent.m_EventType = SceneEventType::SCENE_ONCONSTRUCT;
	constructEvent.m_ObjectClass = SceneObjectClass::SCENE;
	GetApplication()->GetEventDispatcher().Dispatch(constructEvent);
}

void Scene::PrepareFrame()
{
	if(!GetRenderAPI()) return; //cant render with no context

	for (uint i = 0; i < m_Renderers.size(); i++)
	{
		m_Renderers[i]->PrepareFrame();
	}
}

void Scene::RenderFrame(Ref<Camera> camera)
{
	if (!GetRenderAPI()) return; //cant render with no context

	for (uint i = 0; i < m_Renderers.size(); i++)
	{
		m_Renderers[i]->RenderFrame(camera);
	}
}

void Scene::ClearFrame()
{
	if (!GetRenderAPI()) return; //cant render with no context

	for (uint i = 0; i < m_Renderers.size(); i++)
	{
		m_Renderers[i]->ClearFrame();
	}
}

void Scene::CreateDefaultRenderers()
{
	
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
		event.m_EventType = SceneEventType::OBJECT_PRE_DELETE;
		event.m_ObjectClass = SceneObjectClass::SCENEOBJECT;
		event.m_SceneObject = obj;

		//Dispatch PRE_DELETE event
		GetSceneEventDipatcher().Dispatch(event);
		m_SceneObjects.erase(remove);

		//Dispatch POST_DELETE event
  		event.m_EventType = SceneEventType::OBJECT_POST_DELETE;
		GetSceneEventDipatcher().Dispatch(event);
	}
}

void Scene::DestroySceneObject(Ref<SceneObject> obj)
{
	SceneEvent event;
	event.m_EventType = SceneEventType::OBJECT_PRE_DELETE;
	event.m_ObjectClass = SceneObjectClass::SCENEOBJECT;
	event.m_SceneObject = obj.get();

	//Dispatch PRE_DELETE event
	GetSceneEventDipatcher().Dispatch(event);

	m_SceneObjects.erase(std::find(m_SceneObjects.begin(), m_SceneObjects.end(), obj));

	//Dispatch POST_DELETE event
	event.m_EventType = SceneEventType::OBJECT_POST_DELETE;
	GetSceneEventDipatcher().Dispatch(event);
}

void Scene::DestroyRenderer(Renderer* renderer)
{
	auto remove = m_Renderers.end();
	for (auto it = m_Renderers.begin(); it != m_Renderers.end(); it++)
	{
		if (renderer == (*it).get())
		{
			remove = it;
			break;
		}
	}

	if (remove != m_Renderers.end())
	{
		m_Renderers.erase(remove);
	}
}

void Scene::AddSceneObject(SceneObject* obj)
{
	ASSERT(obj);

	SceneEvent event;
	event.m_EventType = SceneEventType::OBJECT_PRE_INITIALIZE;
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
	event.m_EventType = SceneEventType::OBJECT_POST_INITIALIZE;
	GetSceneEventDipatcher().Dispatch(event);
}
