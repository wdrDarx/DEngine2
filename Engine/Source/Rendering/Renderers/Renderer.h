#pragma once
#include "Framework/ObjectBase.h"
#include "Rendering/RenderAPI.h"
#include "Event/Callback.h"
#include "Event/ModuleEvent.h"

class Scene;

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

		virtual void BeginFrame();
		virtual void EndFrame();
		virtual void ClearFrame();

	private:
		Scene* m_Scene = nullptr;
		Callback<ModuleEvent> m_ModuleCallback;
};
