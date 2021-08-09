#pragma once
#include "Core/Core.h"
#include "Framework/ObjectBase.h"
#include "Framework/Property.h"
#include "Framework/Registry.h"
#include "Framework/Scene.h"
#include "Framework/SceneObject.h"
#include "Framework/SceneObject.h"
#include "Assets/Classes/PrefabAsset.h"
#include "Framework/Array.h"

class ObjectUtils
{
	public:
		/*
			polymorphically resets an objects properties by creating a fresh instance of it
			requires object to be registered
		*/
		static void ResetObjectProps(ObjectBase* object, ObjectRegistry& registry);
		static void ResetObjectProp(ObjectBase* object, const std::string& propName, ObjectRegistry& registry);
		static void AddEmptyArrayPropertyElement(Array<bool>* arrayProperty, StructRegistry& registry);

		static void ResetStructProp(StructBase* Struct, const std::string& propName, StructRegistry& registry);

		static Ref<SceneObject> SpawnPrefabInScene(Ref<PrefabAsset> prefabAset, Ref<Scene> scene, const Transform& transform);

		static ObjectClassType GetObjectClassType(Ref<ObjectBase> obj);
};