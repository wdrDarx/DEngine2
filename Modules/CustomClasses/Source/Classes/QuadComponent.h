#pragma once
#include "CustomClasses.h"
#include "CustomSceneObject.h"
#include "TestAsset.h"
#include "DEngine.h"


class CUSTOMCLASSES QuadComponent : public ObjectComponent
{	
public:
	OBJECT_CLASS_DEF(QuadComponent, ObjectComponent)
		OBJECT_PROPS_BEGIN()
			QuadTransform.pos = {0,0, -200.f};
			PROPDEF(QuadTransform, EditAnywhere)
			PROPDEF(QuadSize, EditAnywhere)
			PROPDEF(QuadColor, EditAnywhere)
			PROPDEF(QuadTexture, EditAnywhere)
			PROPDEF_ARRAY(m_Test, GetOwner()->GetScene()->GetApplication()->GetStructRegistry(), EditAnywhere)
		OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;

public:
	Transform QuadTransform;
	vec2d QuadSize = {100,100};
	color4 QuadColor = { 1,1,1,1 };
	AssetRef<TextureAsset> QuadTexture;
	Array<Transform> m_Test;
};

