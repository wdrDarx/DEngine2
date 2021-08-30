#include "PBR_Material.h"
#include "DEngine.h"

void PBR_Material::OnConstruct()
{
	Super::OnConstruct();
	m_BlankTexture = MakeRef<Texture>(TextureSpec());
	
}

void PBR_Material::Bind(Ref<RenderAPI> RenderApi)
{
	if (!RenderApi->IsShaderInCache("PBR_Instanced"))
		RenderApi->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "\\Shaders\\PBR_Instanced.shader"), "PBR_Instanced");

	m_Shader = RenderApi->GetShaderFromCache("PBR_Instanced");
	m_Shader->Bind();

	GetShader()->SetUniform4f("u_Color", Albedo.r, Albedo.g, Albedo.b, Albedo.a);

	//blank texture 
	m_BlankTexture->Bind(0);
	GetShader()->SetUniform1i("u_Albedo", 0);
	GetShader()->SetUniform1i("u_Normal", 0);
	GetShader()->SetUniform1i("u_Metallic", 0);
	GetShader()->SetUniform1i("u_Roughness", 0);

	//multipliers
	GetShader()->SetUniform1f("u_MetallicMult", Metallic);
	GetShader()->SetUniform1f("u_RoughnessMult", Roughness);
	GetShader()->SetUniform1f("u_AmbientLightMult", AmbientMultiplier);

	//bools 
	GetShader()->SetUniform1i("u_HasNormalMap", NormalTexture.IsValid());


	if(auto AlbedoAsset = GetSceneContext()->GetApplication()->GetAssetManager().LoadAsset(AlbedoTexture))
	{ 
		auto AlbedoTex = AlbedoAsset->GetTexture();
		if (AlbedoTex)
		{
			AlbedoTex->Bind(1);
			GetShader()->SetUniform1i("u_Albedo", 1);
		}
	}

	if (auto NormalAsset = GetSceneContext()->GetApplication()->GetAssetManager().LoadAsset(NormalTexture))
	{
		auto NormalTex = NormalAsset->GetTexture();
		if (NormalTex)
		{
			NormalTex->Bind(2);
			GetShader()->SetUniform1i("u_Normal", 2);
		}
	}

	if (auto RoughnessAsset = GetSceneContext()->GetApplication()->GetAssetManager().LoadAsset(RoughnessTexture))
	{
		auto RoughnessTexture = RoughnessAsset->GetTexture();
		if (RoughnessTexture)
		{
			RoughnessTexture->Bind(3);
			GetShader()->SetUniform1i("u_Roughness", 3);
		}
	}

	if (auto MetallicAsset = GetSceneContext()->GetApplication()->GetAssetManager().LoadAsset(MetallicTexture))
	{
		auto MetallicText = MetallicAsset->GetTexture();
		if (MetallicText)
		{
			MetallicText->Bind(4);
			GetShader()->SetUniform1i("u_Metallic", 4);
		}
	}

}