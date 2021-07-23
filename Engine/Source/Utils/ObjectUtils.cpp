#include "ObjectUtils.h"
#include "DEngine.h"

void ObjectUtils::ResetObjectProps(Ref<ObjectBase> object, Registry& registry)
{
	Ref<ObjectBase> obj = ToRef<ObjectBase>(registry.MakeAmbiguous(object->GetClassType().Name));
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

void ObjectUtils::ResetObjectProp(Ref<ObjectBase> object, const std::string& propName, Registry& registry)
{
	Ref<ObjectBase> obj = ToRef<ObjectBase>(registry.MakeAmbiguous(object->GetClassType().Name));
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
			if (Cast<ObjectBase>(obj))
			{
				out = ObjectClassType::OBJECT;
			}

	return out;
}

