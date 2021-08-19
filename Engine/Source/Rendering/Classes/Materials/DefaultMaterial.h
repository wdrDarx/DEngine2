#pragma once
#include "Rendering/Material.h"
#include "Assets/Classes/TextureAsset.h"
#include "Assets/AssetRef.h"

class DENGINE_API DefaultMaterial : public Material
{
	public:
		OBJECT_CLASS_DEF(DefaultMaterial, Material);
		OBJECT_PROPS_BEGIN()
			PROPDEF(Color, EditAnywhere);
			PROPDEF(Albedo, EditAnywhere);
		OBJECT_PROPS_END()

	void OnConstruct() override;
	void Bind(Ref<RenderAPI> RenderApi) override;

public:
	color4 Color = {1,1,1,1};
	AssetRef<TextureAsset> Albedo;
	Ref<Texture> m_BlankTexture;
};
