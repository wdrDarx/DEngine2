#pragma once
#include "CustomSceneObject.h"

struct StringStruct : public StructBase
{
	STRUCT_CLASS_DEF(StringStruct, StructBase)
		STRUCT_PROPS_BEGIN()
			PROPDEF(one, EditAnywhere);
			PROPDEF(two, EditAnywhere);
		STRUCT_PROPS_END()

	std::string one = "shawty bad yeah?";
	std::string two = "a little baddie";
};

class ChildSceneObject : public SceneObject
{
public:
	OBJECT_CLASS_DEF(ChildSceneObject, SceneObject)
		OBJECT_PROPS_BEGIN()
			PROPDEF(QuadTransform, EditAnywhere)
			PROPDEF(QuadColor, EditAnywhere)
		OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;


public:
	Transform QuadTransform;
	color4 QuadColor = {1,0,0,1};
};

