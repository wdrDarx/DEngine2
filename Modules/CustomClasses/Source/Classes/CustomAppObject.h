#pragma once
#include "DEngine.h"

class CustomAppObject : public AppObject
{
public:
	OBJECT_CLASS_DEF(CustomAppObject, AppObject)

	void OnConstruct() override;
};

