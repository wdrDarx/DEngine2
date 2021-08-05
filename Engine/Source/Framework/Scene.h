#pragma once
#include "AppObject.h"
#include "SceneObject.h"
#include "Event/Callback.h"
#include "Event/SceneEvent.h"
#include "Event/EventDispatcher.h"
#include "Rendering/Renderers/Renderer.h"
#include "Framework/InputManager.h"

/*
* An AppObject that contains scoped SceneObjects and relays functions to them
* Contains an array of renderers that can be invoked by scene objects
* Must call Initialize(const ObjectInitializer& initializer) manually after constructing
*/
class Camera;
class DENGINE_API Scene : public AppObject
{
public:
	OBJECT_CLASS_DEF(Scene, AppObject)

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;

	//once per tick after everything is updated
	void PrepareFrame();

	//call for each render target (e.g a viewport)
	void RenderFrame(Ref<Camera> camera);

	// call once after finished rendering to all render targets 
	// (used to clear renderer data)
	void ClearFrame();

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

	//create a renderer from a custom class
	template<class T>
	Ref<T> CreateRenderer(ObjectInitializer& initializer = ObjectInitializer())
	{
		bool valid = std::is_base_of<Renderer, T>::value;
		ASSERT(valid);

		Ref<T> ptr = MakeRef<T>(this);

		Ref<Renderer> obj = Cast<Renderer>(ptr);
		m_Renderers.push_back(obj);

		//Must call this
		initializer.Flags |= ContructFlags::RANDOMID;
		ptr->Initialize(initializer);

		return ptr;
	}

	//try get renderer by class if it exists in this scene
	template<class T>
	Ref<T> GetRenderer()
	{
		bool valid = std::is_base_of<Renderer, T>::value;
		ASSERT(valid);

		for (auto& renderer : m_Renderers)
		{
			if (auto out = Cast<T>(renderer))
			{
				return out;
			}
		}

		ASSERT(false) //renderer not found in this scene;
		return nullptr;
	}

	//set the render api reference (usually from the application)
	void SetRenderAPI(Ref<RenderAPI> api)
	{
		m_RenderApi = api;
	}

	Ref<RenderAPI> GetRenderAPI()
	{
		return m_RenderApi;
	}

	void CreateDefaultRenderers();

	//remove object from Scene Object array
	void DestroySceneObject(Ref<SceneObject> obj);

	//remove object from Scene Object array
	void DestroySceneObject(SceneObject* obj);

	//remove renderer from Scene Object array
	void DestroyRenderer(Renderer* renderer);

	//Will call initialize to an already existing scene object 
	void AddSceneObject(SceneObject* obj);

	EventDispatcher& GetSceneEventDipatcher() 
	{
		return m_EventDispatcher;
	}

	InputManager& GetInputManager()
	{
		return m_InputManager;
	}

private:

	//for scene events
	EventDispatcher m_EventDispatcher;

	//input manager specific to a scene
	InputManager m_InputManager;

	//array of scene objects
	std::vector<Ref<SceneObject>> m_SceneObjects;

	//array of renderers
	std::vector<Ref<Renderer>> m_Renderers;

	//reference to whatever render api (needed for the renderer array)
	Ref<RenderAPI> m_RenderApi;
};