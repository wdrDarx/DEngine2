#include "PBR_Material.h"
#include "DEngine.h"

void PBR_Material::OnConstruct()
{
	Super::OnConstruct();
	m_BlankTexture = MakeRef<Texture>(TextureSpec());
}

void PBR_Material::Bind(Ref<RenderAPI> RenderApi)
{
	bool Deffered = false;

	if (auto pipeline = Cast<DefaultPipeline>(GetSceneContext()->GetPipeline()))
	{
		//use special shader for deferred rendering
		if (pipeline->m_DefferedRendering)
			Deffered = true;
	}

	if (!RenderApi->IsShaderInCache("PBR_Instanced"))
	{	
		Ref<Shader> PBR_Shader;
		if(Deffered)
		{
			PBR_Shader = MakeRef<Shader>(Paths::GetEngineDirectory() + "\\Shaders\\PBR_Instanced_Deferred.vert", Paths::GetEngineDirectory() + "\\Shaders\\PBR_Instanced_Deferred.frag");
		}
		else
			PBR_Shader = MakeRef<Shader>(Paths::GetEngineDirectory() + "\\Shaders\\PBR_Instanced.shader");

		RenderApi->AddShaderToCache(PBR_Shader, "PBR_Instanced");
	}
		

	m_Shader = RenderApi->GetShaderFromCache("PBR_Instanced");
	m_Shader->Bind();

	GetShader()->SetUniform4f("u_Color", Albedo.r, Albedo.g, Albedo.b, Albedo.a);

	//blank texture 
	m_BlankTexture->Bind(0);
	GetShader()->SetUniform1i("u_Albedo", 0);
	GetShader()->SetUniform1i("u_Normal", 0);
	GetShader()->SetUniform1i("u_Metallic", 0);
	GetShader()->SetUniform1i("u_Roughness", 0);
	GetShader()->SetUniform1i("u_CombinedMap", 0);
	GetShader()->SetUniform1i("u_ParallaxMap", 0);

	//multipliers
	GetShader()->SetUniform1f("u_MetallicMult", Metallic);
	GetShader()->SetUniform1f("u_RoughnessMult", Roughness);

	//bools 
	GetShader()->SetUniform1i("u_HasNormalMap", NormalTexture.IsValid());
	//GetShader()->SetUniform1i("u_HasParallaxMap", ParallaxHeightmap.IsValid());
	GetShader()->SetUniform1i("u_UseCombinedMap", UseCombined);

	//inputs
	//GetShader()->SetUniform1f("u_ParallaxHeightScale", ParallaxHeightScale);
	//GetShader()->SetUniform1ui("u_ParallaxMinSteps", ParallaxMinSteps);
	//GetShader()->SetUniform1ui("u_ParallaxMaxSteps", ParallaxMaxSteps);


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

	if (auto CombinedAsset = GetSceneContext()->GetApplication()->GetAssetManager().LoadAsset(CombinedTexture))
	{
		auto CombinedText = CombinedAsset->GetTexture();
		if (CombinedText)
		{
			CombinedText->Bind(5);
			GetShader()->SetUniform1i("u_CombinedMap", 5);
		}
	}

// 	if (auto ParallaxHeightmapAsset = GetSceneContext()->GetApplication()->GetAssetManager().LoadAsset(ParallaxHeightmap))
// 	{
// 		auto ParallaxHeightmapText = ParallaxHeightmapAsset->GetTexture();
// 		if (ParallaxHeightmapText)
// 		{
// 			ParallaxHeightmapText->Bind(6);
// 			GetShader()->SetUniform1i("u_ParallaxMap", 6);
// 		}
// 	}



}
