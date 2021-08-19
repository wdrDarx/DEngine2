#pragma once
#include "Framework/ObjectBase.h"
#include "Framework/FrameworkMacros.h"
#include "Rendering/Shader.h"
#include "Rendering/VertexBufferLayout.h"

/*
	Class for expressing a wrapper around a shader (uniform settings, etc)
	Anything you want to edit can be exposed with properties (including asset refernced to textures and such)
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

protected:
	Ref<Shader> m_Shader;
};