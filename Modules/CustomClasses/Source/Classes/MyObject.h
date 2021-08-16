#pragma once
#include "CustomClasses.h"
#include "DEngine.h"

class CUSTOMCLASSES QuadMaterial : public Material
{
	public: 
		OBJECT_CLASS_DEF(QuadMaterial, Material);
		OBJECT_PROPS_BEGIN()
			PROPDEF(DiffuseTexture, EditAnywhere);
			PROPDEF(Tint, EditAnywhere);
		OBJECT_PROPS_END()

		AssetRef<TextureAsset> DiffuseTexture;
		color4 Tint;
};