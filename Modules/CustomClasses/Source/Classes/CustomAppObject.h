#pragma once
#include "DEngine.h"

class CustomAppObject : public AppObject
{
public:
	OBJECT_CLASS_DEF(CustomAppObject, AppObject)

	OBJECT_PROPS_BEGIN()
		PROPDEF(testProp, PropFlags::NoSerialize)
		PROPDEF(testProp2, 0)
	OBJECT_PROPS_END()
	
	
	void OnConstruct() override;


	float testProp = 12.f;
	vec2d testProp2;
};

