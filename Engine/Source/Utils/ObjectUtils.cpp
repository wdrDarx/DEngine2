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

void ObjectUtils::ResetObjectProp(SceneObject* object, const std::string& propName, Ref<PrefabAsset> prefabAsset)
{
	ArrayBuffer propArrayBuffer;
	{
		Buffer& objBuffer = prefabAsset->m_SceneObjectBuffer;
		STARTREAD(objBuffer, 0);
		UID id;
		std::string name;
		READ(&id, sizeof(UID));
		READSTRING(name);
		ObjectFlags objectFlags;
		READ(&objectFlags, sizeof(ObjectFlags));

		Buffer PropBuffer;
		READBUFFER(PropBuffer);
		propArrayBuffer.FromBuffer(PropBuffer);
	}

	for (auto& prop : object->GetPropertiesMutable())
	{
		if (prop.m_name == propName)
		{
			for (auto& piece : propArrayBuffer.GetDataPieces())
			{
				Property defaultProp;
				defaultProp.LoadAllMetadata(piece);

				if (prop.m_name == defaultProp.m_name && prop.m_Type == defaultProp.m_Type)
				{
					prop.FromBuffer(piece);
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
			struc->GetPropertiesMutable().clear();
 			newElem.Assign(struc, arrayProperty->m_ElementSize);
			
			//delete as bytes to not call destructor
			delete[] (byte*)struc;			
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
				newElem.m_Value = mem;
				newElem.m_ValueSize = arrayProperty->m_ElementSize;
			}

	//add the array element
	arrayProperty->m_InternalArray.push_back(newElem);
}

void ObjectUtils::ResetStructProp(StructBase* Struct, const std::string& propName, StructRegistry& registry)
{
	//create a base class copy
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

Buffer ObjectUtils::GenerateStaticPropsBuffer(const std::vector<StaticProperty>& StaticProps)
{
	ArrayBuffer PropBuffer;
	for (auto& prop : StaticProps)
	{
		PropBuffer.AddPiece(prop.MakeBuffer());
	}
	return PropBuffer.MakeBuffer();
}

std::vector<StaticProperty> ObjectUtils::GenerateStaticPropsFromBuffer(const Buffer& buffer, StructRegistry& structRegistry)
{
	std::vector<StaticProperty> outProps;
	ArrayBuffer PropBuffer;
	PropBuffer.FromBuffer(buffer);
	for (auto& piece : PropBuffer.GetDataPieces())
	{
		StaticProperty prop;
		prop.FromStaticBuffer(piece, structRegistry);
		outProps.push_back(prop);
	}

	return outProps;
}

