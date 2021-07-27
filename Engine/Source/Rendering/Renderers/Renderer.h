#pragma once
#include "Framework/ObjectBase.h"
#include "Rendering/RenderAPI.h"
#include "Event/Callback.h"
#include "Event/ModuleEvent.h"

class Scene;
class Camera;
/*
* inheritable custom class for modules to implement custom rendering (auto destroyed when a module unloads)
* exists in the scope of a scene
*/
class DENGINE_API Renderer : public ObjectBase
{
	public:	
		OBJECT_CLASS_DEF(Renderer, ObjectBase)

		//override constructor receiving a reference to the scene
		Renderer(Scene* scene) : ObjectBase(), m_Scene(scene)
		{

		}

		Scene* GetScene()
		{
			return m_Scene;
		}

		void OnConstruct() override;

		//called after every object ran their update function so its safe to handle data from there (CALLED ONCE PER TICK)
		virtual void PrepareFrame();

		//called for every camera that wants a render of this renderer
		virtual void RenderFrame(Ref<Camera> camera);

		//called once per tick after all the frames are rendered and data is safe to discard
		virtual void ClearFrame();

	private:
		Scene* m_Scene = nullptr;
		Callback<ModuleEvent> m_ModuleCallback;
};
