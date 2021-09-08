#pragma once
#include "Rendering/Material.h"
#include "Assets/Classes/TextureAsset.h"
#include "Assets/AssetRef.h"
#include "Framework/Array.h"

class DENGINE_API PBR_Material : public Material
{
	public:
		OBJECT_CLASS_DEF(PBR_Material, Material);
		OBJECT_PROPS_BEGIN()
			PROPS_CATEGORY(Material);
				PROPDEF(Albedo, EditAnywhere);
				PROPDEF(AlbedoTexture, EditAnywhere);
				PROPDEF(NormalTexture, EditAnywhere);
				PROPDEF(Roughness, EditAnywhere);
				PROPDEF(RoughnessTexture, EditAnywhere);
				PROPDEF(Metallic, EditAnywhere);
				PROPDEF(MetallicTexture, EditAnywhere);
				PROPDEF(UseCombined, EditAnywhere);
				PROPDEF(CombinedTexture, EditAnywhere);
		OBJECT_PROPS_END()

	void OnConstruct() override;
	void Bind(Ref<RenderAPI> RenderApi) override;

public:
	color4 Albedo = {1,1,1,1};
	AssetRef<TextureAsset> AlbedoTexture;

	AssetRef<TextureAsset> NormalTexture;

	float Roughness = 1.0f;
	AssetRef<TextureAsset> RoughnessTexture;

	float Metallic = 0.f;
	AssetRef<TextureAsset> MetallicTexture;

	bool UseCombined = false;
	AssetRef<TextureAsset> CombinedTexture;

	Ref<Texture> m_BlankTexture;
};
