#include "DefaultMaterial.h"
#include "DEngine.h"

void DefaultMaterial::OnConstruct()
{
	Super::OnConstruct();
	m_BlankTexture = MakeRef<Texture>(TextureSpec());
	
}

void DefaultMaterial::Bind(Ref<RenderAPI> RenderApi)
{
	if (!RenderApi->IsShaderInCache("DefaultMaterial"))
		RenderApi->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "\\Shaders\\DefaultMaterial.shader"), "DefaultMaterial");

	m_Shader = RenderApi->GetShaderFromCache("DefaultMaterial");
	m_Shader->Bind();

	GetShader()->SetUniformMat4f("u_ViewProjectionMatrix", RenderApi->GetCamera()->GetViewProjectionMatrix());
	GetShader()->SetUniform4f("u_Color", Color.r, Color.g, Color.b, Color.a);

	//blank texture 
	m_BlankTexture->Bind(0);
	GetShader()->SetUniform1i("u_Albedo", 0);


	if(auto AlbedoAsset = GetSceneContext()->GetApplication()->GetAssetManager().LoadAsset(Albedo))
	{ 
		auto AlbedoTex = AlbedoAsset->GetTexture();
		if (AlbedoTex)
		{
			AlbedoTex->Bind(1);
			GetShader()->SetUniform1i("u_Albedo", 1);
		}
	}

}
