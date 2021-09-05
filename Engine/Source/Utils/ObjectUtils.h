#pragma once
#include "Core/Core.h"
#include "Framework/ObjectBase.h"
#include "Framework/Property.h"
#include "Framework/Registry.h"
#include "Framework/SceneObject.h"
#include "Assets/Classes/PrefabAsset.h"
#include "Framework/Array.h"

struct RemoveResult
{
	uint RemovedIndex;
	bool WasRemoved;
};

/*
	Uitily class for Objects, Structs and properties
*/
class ObjectUtils
{
	public:
		/*
			polymorphically resets an objects properties by creating a fresh instance of it
			requires object to be registered
		*/
		static void ResetObjectProps(ObjectBase* object, ObjectRegistry& registry);

		/*
			Resets an object property based on its name
		*/
		static void ResetObjectProp(ObjectBase* object, const std::string& propName, ObjectRegistry& registry);

		/*
			Resets an SceneObject property based on its name
			Takes a reference to a prefab asset to load the default property based on the asset property
		*/
		static void ResetObjectProp(SceneObject* object, const std::string& propName, Ref<PrefabAsset> prefabAsset);

		/*
			Resets a structs property based on its name
		*/
		static void ResetStructProp(StructBase* Struct, const std::string& propName, StructRegistry& registry);


		/*
			Appends a new empty element to an array
		*/
		static void AddEmptyArrayPropertyElement(Array<bool>* arrayProperty, StructRegistry& registry);

		/*
			Returns the ObjectClassType from a polymorphic pointer to an object
		*/
		static ObjectClassType GetObjectClassType(Ref<ObjectBase> obj);

		/*
			Creates a buffer from static properties
		*/
		static Buffer GenerateStaticPropsBuffer(const std::vector<StaticProperty>& StaticProps);

		/*
			Creates an array of static properties from a buffer (Needs a struct registry)
		*/
		static std::vector<StaticProperty> GenerateStaticPropsFromBuffer(const Buffer& buffer, StructRegistry& structRegistry);

		/*
			returns the erased index
		*/
		template<class T>
		static RemoveResult RemovePointerFromRefVector(T* Object, std::vector<Ref<T>>& vector);

		template<typename T>
		static RemoveResult RemoveFromVector(const T& object, std::vector<T>& vector);
};

template<class T>
RemoveResult ObjectUtils::RemovePointerFromRefVector(T* Object, std::vector<Ref<T>>& vector)
{
	auto remove = vector.end();
	for (auto it = vector.begin(); it != vector.end(); it++)
	{
		if ((*it).get() == Object)
			remove = it;
	}
	if (remove != vector.end())
	{
		uint erased = std::distance(vector.begin(), remove);
		vector.erase(remove);
		return { erased , true };
	}

	return { 0, false };
}

template<typename T>
RemoveResult ObjectUtils::RemoveFromVector(const T& Object, std::vector<T>& vector)
{
	auto remove = std::find(vector.begin(), vector.end(), Object);
	if (remove != vector.end())
	{ 
		uint erased = std::distance(vector.begin(), remove);
		vector.erase(remove);
		return { erased , true};
	}

	return {0, false};
}
