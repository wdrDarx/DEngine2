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

