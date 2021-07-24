#pragma once
#include "Core/Core.h"
#include "Serialization/Buffer.h"
#include "Event/EditorCallback.h"
#include <variant>

enum PropFlags
{	
	NoSerialize = BIT(0),
	EditAnywhere = BIT(1)
};

enum class PropType
{
	BOOL = 0,
	INT,
	FLOAT,
	STRING,
	VEC2D,
	VEC3D,
	STRUCT
};

/*
	these are tied to an object class/instance 
	(the value is a pointer to an already constructed variable)
	if you need properties that hold their own values use StaticProperty
*/
struct Property
{
	std::string m_name;
	std::string m_category;
	PropType m_Type;
	int m_Flags;
	void* m_Value;
	size_t m_ValueSize;

	Property()
	{

	}

	virtual ~Property()
	{

	}

	Property(const std::string& name, const std::string& category,
		PropType type, void* value, const size_t& valueSize, int flags = 0) : m_name(name),
		m_category(category), m_Type(type), m_Flags(flags), m_Value(value), m_ValueSize(valueSize)
	{

	}

	//serializes into a single byte stream
	Buffer MakeBuffer() const;

	//the props already exist with an address, DO NOT delete them or use "new", just cast them to their type
	void FromBuffer(const Buffer& buffer);

	//use for loading other props
	void LoadNameAndType(const Buffer& buffer)
	{
		STARTREAD(buffer, 0);
		READSTRING(m_name);
		READ(&m_Type, sizeof(PropType));
	}
};

//only difference is that the Value pointer is tied to this property instance and not an external class,
//its therefor also deleted by ~StaticProperty()
struct StaticProperty : public Property
{
	using Property::Property;

	//need a copy constructor to explicitly copy the value of this property
	StaticProperty(const StaticProperty& other)
	{
		m_name = other.m_name;
		m_category = other.m_category;
		m_Type = other.m_Type;
		m_Flags = other.m_Flags;

		m_Value = new byte[other.m_ValueSize];
		memcpy(m_Value, other.m_Value, other.m_ValueSize);

		m_ValueSize = other.m_ValueSize;
	}

	StaticProperty(StaticProperty&& other)
	{
		ASSERT(false) //dont wanna be calling this cuz idk what it will do

		m_name = other.m_name;
		m_category = other.m_category;
		m_Type = other.m_Type;
		m_Flags = other.m_Flags;

		m_Value = new byte[other.m_ValueSize];
		memcpy(m_Value, other.m_Value, other.m_ValueSize);

		m_ValueSize = other.m_ValueSize;
	}

	~StaticProperty()
	{
		if(m_Value)
			delete m_Value;
	}
};


