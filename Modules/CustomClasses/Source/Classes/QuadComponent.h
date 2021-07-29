#pragma once
#include "CustomClasses.h"
#include "TextureAsset.h"
#include "CustomSceneObject.h"
#include "TestAsset.h"
#include "DEngine.h"

enum WeekEnd
{
	Sunday = 1,
	Saturday = 7
};

// String support for WeekEnd
Begin_Enum_String(WeekEnd)
{
	Enum_String(Sunday);
	Enum_String(Saturday);
}
End_Enum_String;


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
			PROPDEF(MessageAsset, EditAnywhere)
			PROPDEF(m_Enum, EditAnywhere)
			PROPDEF(m_TestArray, EditAnywhere)
		OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;

public:
	Transform QuadTransform;
	vec2d QuadSize = {100,100};
	color4 QuadColor = { 1,0,0,1 };
	AssetRef<TextureAsset> QuadTexture;
	AssetRef<TestAsset> MessageAsset;
	WeekEnd m_Enum;
	Array<std::string> m_TestArray;
};

