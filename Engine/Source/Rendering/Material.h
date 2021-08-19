#pragma once
#include "Framework/ObjectBase.h"
#include "Framework/FrameworkMacros.h"
#include "Rendering/Shader.h"
#include "Rendering/VertexBufferLayout.h"

class Scene;
/*
	Class for expressing a wrapper around a shader (uniform settings, etc)
	Anything you want to edit can be exposed with properties (including asset refernces to textures and such)
*/
class DENGINE_API Material : public ObjectBase
{
public:
	OBJECT_CLASS_DEF(Material, ObjectBase);

	/*
		Gets called by the renderer
	*/
	virtual void Bind(Ref<RenderAPI> renderApi)
	{
		
	}

	virtual void Unbind()
	{
	
	}

	Ref<Shader> GetShader()
	{
		return m_Shader;
	}

	//scene context could be null!
	Scene* GetSceneContext() const
	{
		ASSERT(m_Scene); //context is null
		return m_Scene;
	}

	void SetSceneContext(Scene* SceneContext)
	{
		ASSERT(SceneContext); //context is null
		m_Scene = SceneContext;
	}

protected:
	Ref<Shader> m_Shader;

	//context for a bunch of stuff
	Scene* m_Scene;
};