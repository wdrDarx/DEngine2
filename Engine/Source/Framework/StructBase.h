#pragma once
#include "Core/Core.h"
#include "Serialization/Buffer.h"
#include "Property.h"
#include "StaticClass.h"
#include "FrameworkMacros.h"

/*
	Base Class for all structs
*/
struct DENGINE_API StructBase
{
public:
	//exists to prevent property redefinition by inheritance since this class doesnt have a seperate "Initialize" method
	StructBase(bool DefProps)
	{
		if (DefProps)
			DefineProperties();
	}
	//dont explicitly call, properties are defined on construction
	virtual void DefineProperties()
	{

	}

	//sets the size through a v-table to the last class
	virtual void CalculateMySize()
	{
		
	}

public:

	StructBase()
	{
		
	}

	~StructBase()
	{
	
	}

	//get static class - use STRUCT_STATIC_CLASS()
	template<class T>
	static StaticClass _GetStaticClass()
	{
		StaticClass out;
		out.FromTemlate<T>();
		return out;
	}

	const std::vector<Property>& GetProperties() const
	{
		return m_Properties;
	}

	std::vector<Property>& GetPropertiesMutable()
	{
		return m_Properties;
	}

	//overriden by macros
	virtual ClassType GetClassType() const
	{
		return typeid(this);
	};

	void SerializeProps(ArrayBuffer& buffer) const;

	//only call after DefineProperties()
	void LoadProps(const ArrayBuffer& buffer);

	const size_t& GetSize() const
	{
		return m_MySize;
	}

protected:
	std::vector<Property> m_Properties;

	//set by the struct_class_def macro, used for property stuff
	size_t m_MySize = 0;
};