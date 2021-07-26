#pragma once
#include "CustomSceneObject.h"
class ChildSceneObject : public CustomSceneObject
{
public:
	OBJECT_CLASS_DEF(ChildSceneObject, CustomSceneObject)
		OBJECT_PROPS_BEGIN()
			PROPDEF(childProp, EditAnywhere)
			PROPDEF(QuadPos, EditAnywhere)
		OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;
public:
	float childProp = 0.f;
	vec2d QuadPos;
};

