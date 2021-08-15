#pragma once
#include "CustomClasses.h"
#include "DEngine.h"

class CUSTOMCLASSES MyObject : public ObjectBase
{
	public: 
		OBJECT_CLASS_DEF(MyObject, ObjectBase);
		OBJECT_PROPS_BEGIN()
			PROPDEF(test1, EditAnywhere);
			PROPDEF_ARRAY(test2, Module::GetStaticApplication()->GetStructRegistry(), EditAnywhere);
		OBJECT_PROPS_END()

		std::string test1 = "Ok and?";
		Array<std::string> test2;
};