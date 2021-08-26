#include "Scene.h"
#include "DEngine.h"

void Scene::OnBeginPlay()
{
	Super::OnBeginPlay();

	for (auto& obj : GetSceneObjects())
	{
		obj->OnBeginPlay();
	}
}

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
	//CreateDefaultRenderers();

	SceneEvent constructEvent;
	constructEvent.m_Scene = this;
	constructEvent.m_EventType = SceneEventType::SCENE_ONCONSTRUCT;
	constructEvent.m_ObjectClass = SceneObjectClass::SCENE;
	GetApplication()->GetEventDispatcher().Dispatch(constructEvent);
}

void Scene::OnDestroy()
{
	Super::OnDestroy();

	//call on destroy for all scene objects
	for (auto& obj : GetSceneObjects())
	{
		obj->OnDestroy();
	}
}

uint Scene::Serialize(Buffer& buffer) const
{
	STARTWRITE(buffer, Super::Serialize(buffer));
	ArrayBuffer SceneObjectBuffer;
	for (auto& obj : GetSceneObjects())
	{
		//the object doesnt serialize its class type, which we need to know to create an instance of it on deserialization
		[&]()
		{
			Buffer FinalObjBuffer;
			STARTWRITE(FinalObjBuffer, 0);
			std::string ObjClassName = obj->GetClassType().Name;
			WRITESTRING(ObjClassName);
			Buffer ObjDataBuffer;
			obj->Serialize(ObjDataBuffer);
			WRITEBUFFER(ObjDataBuffer);

			SceneObjectBuffer.AddPiece(FinalObjBuffer);
		}();
	}
	WRITEBUFFER(SceneObjectBuffer.MakeBuffer());
	STOPWRITE();
}

uint Scene::Deserialize(const Buffer& buffer)
{
	STARTREAD(buffer, Super::Deserialize(buffer));
	Buffer SceneObjectBuffer;
	READBUFFER(SceneObjectBuffer);
	ArrayBuffer SceneObjectBufferArray;
	SceneObjectBufferArray.FromBuffer(SceneObjectBuffer);

	for (uint i = 0; i < SceneObjectBufferArray.GetDataPieces().size(); i++)
	{
		[&]()
		{
			const Buffer& piece = SceneObjectBufferArray.GetDataPieces()[i];
			STARTREAD(piece, 0);
			std::string ObjClassName;
			READSTRING(ObjClassName);
			ObjectBase* test = GetApplication()->GetObjectRegistry().MakeObjectFromClassName(ObjClassName);
			auto sceneObject = ToRef<SceneObject>(Cast<SceneObject>(test));
			ASSERT(sceneObject); //either class is not registered (check if the associated module is loaded maybe) or the class name is invalid
			Buffer ObjBuffer;
			READBUFFER(ObjBuffer);
			AddSceneObject(sceneObject, ObjectInitializer(ConstructFlags::NOPOSTCONSTRUCT | ConstructFlags::RANDOMID)); //dont call on post construct yet
			sceneObject->Deserialize(ObjBuffer);
			sceneObject->OnPostConstruct(); //now after deserializing call on post construct
		}();
	}
	STOPREAD();
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

		//call on destroy
		(*remove)->OnDestroy();
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

	//call on destroy
	obj->OnDestroy();

	//finally erase
	m_SceneObjects.erase(std::find(m_SceneObjects.begin(), m_SceneObjects.end(), obj));

	//Dispatch POST_DELETE event
	event.m_EventType = SceneEventType::OBJECT_POST_DELETE;
	GetSceneEventDipatcher().Dispatch(event);
}

void Scene::AddSceneObject(SceneObject* obj, const ObjectInitializer& Initializer)
{
	ASSERT(obj);

	SceneEvent event;
	event.m_EventType = SceneEventType::OBJECT_PRE_INITIALIZE;
	event.m_ObjectClass = SceneObjectClass::SCENEOBJECT;
	event.m_SceneObject = obj;
	//Dispatch PRE_INITIALIZE event
	GetSceneEventDipatcher().Dispatch(event);

	//assign scene and add to scene object array
	obj->m_Scene = (this);
	Ref<SceneObject> objref = ToRef<SceneObject>(obj);
	m_SceneObjects.push_back(objref);

	//Must call
	obj->Initialize(Initializer);

	//Dispatch POST_INITIALIZE event
	event.m_EventType = SceneEventType::OBJECT_POST_INITIALIZE;
	GetSceneEventDipatcher().Dispatch(event);
}

void Scene::AddSceneObject(Ref<SceneObject> obj, const ObjectInitializer& Initializer)
{
	ASSERT(obj);

	SceneEvent event;
	event.m_EventType = SceneEventType::OBJECT_PRE_INITIALIZE;
	event.m_ObjectClass = SceneObjectClass::SCENEOBJECT;
	event.m_SceneObject = obj.get();
	//Dispatch PRE_INITIALIZE event
	GetSceneEventDipatcher().Dispatch(event);

	//assign scene and add to scene object array
	obj->m_Scene = (this);
	m_SceneObjects.push_back(obj);

	//Must call
	obj->Initialize(Initializer);

	//Dispatch POST_INITIALIZE event
	event.m_EventType = SceneEventType::OBJECT_POST_INITIALIZE;
	GetSceneEventDipatcher().Dispatch(event);
}
