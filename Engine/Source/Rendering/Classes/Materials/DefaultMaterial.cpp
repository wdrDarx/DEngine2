#include "DefaultMaterial.h"
#include "DEngine.h"

void DefaultMaterial::OnConstruct()
{
	Super::OnConstruct();
	
}

void DefaultMaterial::Bind(Ref<RenderAPI> RenderApi)
{
	if (!RenderApi->IsShaderInCache("DefaultMaterial"))
		RenderApi->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "\\Shaders\\DefaultMaterial.shader"), "DefaultMaterial");

	m_Shader = RenderApi->GetShaderFromCache("DefaultMaterial");
	m_Shader->Bind();

	GetShader()->SetUniformMat4f("u_ViewProjectionMatrix", RenderApi->GetCamera()->GetViewProjectionMatrix());
	GetShader()->SetUniform4f("u_Color", Color.r, Color.g, Color.b, Color.a);
}
