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
		static void ResetObjectProps(ObjectBase* object, ObjectRegistry& registry);
		static void ResetObjectProp(ObjectBase* object, const std::string& propName, ObjectRegistry& registry);

		static void ResetStructProp(StructBase* Struct, const std::string& propName, StructRegistry& registry);

		static ObjectClassType GetObjectClassType(Ref<ObjectBase> obj);
};