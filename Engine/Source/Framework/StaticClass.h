#pragma once
#include "Core/Core.h"
#include "Framework/Property.h"


//future implementation
// struct StaticProperty
// {
// 
// };

using StaticProperty = Property;
class ObjectBase;


#ifdef STATICCLASS_DEPRECATED
class DENGINE_API StaticClass
{
public:	
	StaticClass(Ref<ObjectBase> obj);

	StaticClass();

	//need to manually deallocate all the static prop values
	~StaticClass();

	const std::vector<StaticProperty>& GetDefaultProperties() const
	{
		return m_StaticProperties;
	}

	const ClassType& GetClassType() const
	{
		return *m_ClassType;
	}

	template<class T>
	void FromTemlate()
	{
		bool valid = std::is_base_of<ObjectBase, T>::value;
		ASSERT(valid);

		//create an instance and create the static class
		Ref<T> newObj = MakeRef<T>();
		CreateStaticClass(newObj);
	}


private:
	void CreateStaticClass(Ref<ObjectBase> obj);
		
	std::vector<StaticProperty> m_StaticProperties;
	Ref<ClassType> m_ClassType;
	ObjectClassType m_ObjectClassType;
};
#endif

//stores things like default property values, and object registry type by hosting a scoped instance of the obejct
class DENGINE_API StaticClass
{
public:
	StaticClass()
	{

	}

	const std::vector<StaticProperty>& GetDefaultProperties() const;

	const ClassType& GetClassType() const
	{
		return *m_ClassType;
	}

	const ObjectClassType& GetObjectClassType() const
	{
		return m_ObjectClassType;
	}

	template<class T>
	void FromTemlate()
	{
		bool valid = std::is_base_of<ObjectBase, T>::value;
		ASSERT(valid);

		//create an instance and create the static class
		m_ObjectRef = MakeRef<T>();
		CreateStaticClass();
	}

private:
	void CreateStaticClass();

	Ref<ObjectBase> m_ObjectRef;
	Ref<ClassType> m_ClassType;
	ObjectClassType m_ObjectClassType;
};