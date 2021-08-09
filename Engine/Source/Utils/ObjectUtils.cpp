#include "ObjectUtils.h"
#include "DEngine.h"

void ObjectUtils::ResetObjectProps(ObjectBase* object, ObjectRegistry& registry)
{
	Ref<ObjectBase> obj = ToRef<ObjectBase>(registry.MakeObjectFromClassName(object->GetClassType().Name));
	obj->DefineProperties();

	for (auto& prop : object->GetPropertiesMutable())
	{
		for (const auto& defaultProp : obj->GetProperties())
		{
			if (prop.m_name == defaultProp.m_name && prop.m_Type == defaultProp.m_Type)
			{
				prop.FromBuffer(defaultProp.MakeBuffer());
				break;
			}
		}
		
	}
}

void ObjectUtils::ResetObjectProp(ObjectBase* object, const std::string& propName, ObjectRegistry& registry)
{
	Ref<ObjectBase> obj = ToRef<ObjectBase>(registry.MakeObjectFromClassName(object->GetClassType().Name));
	obj->DefineProperties();

	for (auto& prop : object->GetPropertiesMutable())
	{
		if (prop.m_name == propName)
		{
			for (const auto& defaultProp : obj->GetProperties())
			{
				if (prop.m_name == defaultProp.m_name && prop.m_Type == defaultProp.m_Type)
				{
					prop.FromBuffer(defaultProp.MakeBuffer());
					break;
				}
			}
		}

	}
}

void ObjectUtils::AddEmptyArrayPropertyElement(Array<bool>* arrayProperty, StructRegistry& registry)
{
	//construct a new element based on the element class name if its not a regular type
	StaticProperty newElem;
	newElem.m_Type = arrayProperty->m_ElementType;

	if (arrayProperty->m_ElementType == PropType::STRING)
	{		
		std::string string;
		newElem.Assign(&string, sizeof(string));
	}
	else
		if (arrayProperty->m_ElementType == PropType::STRUCT)
		{
			//create struct with the correct class 
			StructBase* struc = registry.Make({ arrayProperty->m_ElementClassName });
			newElem.Assign(struc, sizeof(*struc));

			//delete struc; //beacuse assign copies the pointer
		}
		else
			if (arrayProperty->m_ElementType == PropType::ASSETREF)
			{
				AssetRef<Asset> asset;
				newElem.Assign(&asset, sizeof(asset));
			} 
			else
			{
				//allocate the value bytes
				void* mem = new byte[arrayProperty->m_ElementSize];
				memset(mem, 0, arrayProperty->m_ElementSize);
				newElem.Assign(mem, arrayProperty->m_ElementSize);
			}

	//add the array element
	arrayProperty->m_InternalArray.push_back(newElem);
}

void ObjectUtils::ResetStructProp(StructBase* Struct, const std::string& propName, StructRegistry& registry)
{
	Ref<StructBase> str = ToRef<StructBase>(registry.Make({Struct->GetClassType().Name}));

	for (auto& prop : Struct->GetPropertiesMutable())
	{
		if (prop.m_name == propName)
		{
			for (const auto& defaultProp : str->GetProperties())
			{
				if (prop.m_name == defaultProp.m_name && prop.m_Type == defaultProp.m_Type)
				{
					prop.FromBuffer(defaultProp.MakeBuffer());
					break;
				}
			}
		}

	}
}

Ref<SceneObject> ObjectUtils::SpawnPrefabInScene(Ref<PrefabAsset> prefabAset, Ref<Scene> scene, const Transform& transform)
{	
	auto sceneObject = ToRef<SceneObject>(Cast<SceneObject>(scene->GetApplication()->GetObjectRegistry().MakeObjectFromClassName(prefabAset->GetPrefabBaseClassName())));
	//ASSERT(sceneObject); //object class invalid or not registered
	if(!sceneObject) return nullptr;

	scene->AddSceneObject(sceneObject);

	//load the prefab data
	prefabAset->LoadPrefab(sceneObject, false);

	return sceneObject;
}

ObjectClassType ObjectUtils::GetObjectClassType(Ref<ObjectBase> obj)
{
	ObjectClassType out;
	if (Cast<AppObject>(obj))
	{
		out = ObjectClassType::APPOBJECT;
	}
	else
		if (Cast<SceneObject>(obj))
		{
			out = ObjectClassType::SCENEOBJECT;
		}
		else
			if (Cast<ObjectComponent>(obj))
			{
				out = ObjectClassType::OBJECTCOMPONENT;
			}
			else
				if (Cast<ObjectBase>(obj))
				{
					out = ObjectClassType::OBJECT;
				}

	return out;
}

