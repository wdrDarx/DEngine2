#pragma once
#include "CustomClasses.h"
#include "CustomSceneObject.h"
#include "DEngine.h"



struct CUSTOMCLASSES EnumTest : public EnumBase
{
	enum Enum
	{
		TestEnum1,
		TestEnum2
	};

	ENUM_DEF_BEGIN(EnumTest, Enum);	
		ENUM_DEF(TestEnum1);
		ENUM_DEF(TestEnum2);
	ENUM_DEF_END()
};

struct CUSTOMCLASSES BitmaskTest : public EnumBase
{
	enum Enum
	{
		TestEnum1 = BIT(1),
		TestEnum2 = BIT(2)
	};

	BITMASK_DEF_BEGIN(BitmaskTest, Enum);
		ENUM_DEF(TestEnum1);
		ENUM_DEF(TestEnum2);
	ENUM_DEF_END()
};

class CUSTOMCLASSES QuadComponent : public ObjectComponent
{	
public:
	OBJECT_CLASS_DEF(QuadComponent, ObjectComponent)
		OBJECT_PROPS_BEGIN()
			BaseQuadTransform.pos = {0,0, -200.f};
			PROPDEF(BaseQuadTransform, EditAnywhere)
			PROPDEF(QuadAmount, EditAnywhere)
			PROPDEF(QuadSize, EditAnywhere)
			PROPDEF(QuadColor, EditAnywhere)
			PROPDEF(QuadTexture, EditAnywhere)
			PROPDEF(m_TestEnum, EditAnywhere)
			PROPDEF(m_TestEnum2, EditAnywhere)
			PROPDEF_ARRAY(m_Test, Module::GetStaticApplication()->GetStructRegistry(), EditAnywhere)
		OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;

public:
	Transform BaseQuadTransform;
	Transform QuadTransform;
	vec2d QuadSize = {100,100};
	int QuadAmount = 1;
	color4 QuadColor = { 1,1,1,1 };
	AssetRef<TextureAsset> QuadTexture;
	Array<TestStruct> m_Test;
	float RotationSpeed = 0.f;

	EnumTest m_TestEnum;
	BitmaskTest m_TestEnum2;
};

