#include "StaticClass.h"
#include "Framework/ObjectBase.h"
#include "Framework/AppObject.h"
#include "Framework/SceneObject.h"
#include "Utils/ObjectUtils.h"

#ifdef STATICCLASS_DEPRECATED
StaticClass::StaticClass(Ref<ObjectBase> obj)
{
	CreateStaticClass(obj);
}

StaticClass::StaticClass()
{

}

StaticClass::~StaticClass()
{
	//manually free property memory
	for (auto& prop : m_StaticProperties)
	{
		delete prop.m_Value;
	}
}

void StaticClass::CreateStaticClass(Ref<ObjectBase> obj)
{
	//set class type by copy
	m_ClassType = MakeRef<ClassType>(obj->GetClassType());

	//determine ObjectClassType
	m_ObjectClassType = ObjectUtils::GetObjectClassType(obj);

	//define properties for retrieval
	obj->DefineProperties();

	//copy all property values to StaticProperty array
	for (auto& OriginalProp : obj->GetProperties())
	{
		StaticProperty CopyProp;

		//allocate value bytes
		CopyProp.m_Value = new byte[OriginalProp.m_ValueSize];

		//gen buffer from original props
		Buffer OriginalBuffer = OriginalProp.MakeBuffer();

		//load type mainly
		CopyProp.LoadNameAndType(OriginalBuffer);

		//call constructor for the value (necessary only for DStruct and String)
		if (CopyProp.m_Type == PropType::STRING)
		{
			std::allocator<std::string>().construct((std::string*)CopyProp.m_Value);
		}
		else
		//this is why this version is broken, the DStruct that needs to be constructed
		//should be the actual class derived from DStruct but we cant know what that unless we use a template
		// so instead the new implementation is just a scoped instance of the object
		if (CopyProp.m_Type == PropType::DSTRUCT)
		{
			std::allocator<DStruct>().construct((DStruct*)CopyProp.m_Value);
		}

		//now its safe to copy memory
		CopyProp.FromBuffer(OriginalBuffer);
	}
}



#endif
void StaticClass::CreateStaticClass()
{
	//set class type by copy
	m_ClassType = MakeRef<ClassType>(m_ObjectRef->GetClassType());

	//determine ObjectClassType
	m_ObjectClassType = ObjectUtils::GetObjectClassType(m_ObjectRef);

	//define properties for retrieval
	m_ObjectRef->DefineProperties();
}

const std::vector<StaticProperty>& StaticClass::GetDefaultProperties() const
{
	return m_ObjectRef->GetProperties();
}

