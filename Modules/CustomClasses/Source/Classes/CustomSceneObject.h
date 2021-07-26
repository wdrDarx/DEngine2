#pragma once
#include "CustomClasses.h"
#include "DEngine.h"

struct CUSTOMCLASSES TestStruct : public StructBase
{
	STRUCT_CLASS_DEF(TestStruct, StructBase)
		STRUCT_PROPS_BEGIN()
			PROPDEF(porp, EditAnywhere);
			//PROPDEF(porp2, EditAnywhere);
		STRUCT_PROPS_END()
	
	float porp = 12.f;
};

struct CUSTOMCLASSES TestStruct2 : public TestStruct
{
	STRUCT_CLASS_DEF(TestStruct2, TestStruct)
		STRUCT_PROPS_BEGIN()
			PROPDEF(childProp, EditAnywhere);
			PROPDEF(childProp2, EditAnywhere);
			PROPDEF(prop2, EditAnywhere);
		STRUCT_PROPS_END()

	float childProp = 3.f;
	float childProp2 = 5.f;
	TestStruct prop2;
};

class CUSTOMCLASSES CustomSceneObject : public SceneObject
{
public:
	OBJECT_CLASS_DEF(CustomSceneObject, SceneObject)
		OBJECT_PROPS_BEGIN()
			PROPS_CATEGORY(TestCategory)
				PROPDEF(trans, EditAnywhere);
				PROPDEF(test1, EditAnywhere);
				PROPDEF(test2, EditAnywhere);
				PROPDEF(test3, EditAnywhere);
			PROPS_CATEGORY(TestCategory2)
 				PROPDEF(test4, EditAnywhere);
 				PROPDEF(test5, EditAnywhere);
				PROPDEF(test6, EditAnywhere);
				PROPDEF(struc, EditAnywhere);
		OBJECT_PROPS_END()

	void OnConstruct() override;
	void OnUpdate(const Tick& tick) override;

	void testfunc(float test);

public:
	Transform trans;
	bool test1 = false;
	int test2 = 0;
	float test3 = 0.f;
	std::string test4 = "dabab6y";
	vec2d test5 = {0,0};
	vec3d test6 = {69,420,21};
	TestStruct struc;
};

