#pragma once
#include "Core/Core.h"
#include "Framework/ObjectBase.h"
#include "Framework/Property.h"
#include "Framework/Registry.h"

class ObjectUtils
{
	public:
		/*
			polymorphically resets an objects properties by creating a fresh instance of it
			requires object to be registered
		*/
		static void ResetObjectProps(Ref<ObjectBase> object, Registry& registry);
		static void ResetObjectProp(Ref<ObjectBase> object, const std::string& propName, Registry& registry);
};