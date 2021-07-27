#pragma once
#include "CustomClasses.h"
#include "TextureAsset.h"
#include "DEngine.h"

class CUSTOMCLASSES QuadComponent : public ObjectComponent
{	
public:
	OBJECT_CLASS_DEF(QuadComponent, ObjectComponent)
		OBJECT_PROPS_BEGIN()
			PROPDEF(QuadTransform, EditAnywhere)
			PROPDEF(QuadSize, EditAnywhere)
			PROPDEF(QuadColor, EditAnywhere)
			PROPDEF(QuadTexture, EditAnywhere)
		OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;


public:
	Transform QuadTransform;
	vec2d QuadSize = {100,100};
	color4 QuadColor = { 1,0,0,1 };
	AssetRef<TextureAsset> QuadTexture;
};

