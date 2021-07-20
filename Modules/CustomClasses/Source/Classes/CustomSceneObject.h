#pragma once
#include "CustomClasses.h"
#include "DEngine.h"

class CUSTOMCLASSES CustomSceneObject : public SceneObject
{
public:
	OBJECT_CLASS_DEF(CustomSceneObject, SceneObject)
		OBJECT_PROPS_BEGIN()
			PROPDEF(test1, EditAnywhere);
			PROPDEF(test2, EditAnywhere);
			PROPDEF(test3, EditAnywhere);
			PROPDEF(test4, EditAnywhere);
			PROPDEF(test5, EditAnywhere);
			PROPDEF(test6, EditAnywhere);
		OBJECT_PROPS_END()

	void OnConstruct() override;
	void OnUpdate(const Tick& tick) override;

public:
	bool test1 = false;
	int test2 = 0;
	float test3 = 0.f;
	std::string test4 = "dabab6y";
	vec2d test5 = {0,0};
	vec3d test6 = {0,0,0};
};

