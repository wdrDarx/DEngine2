#pragma once
#include "Core/Core.h"
#include "Serialization/Buffer.h"
#include "Event/EditorCallback.h"
#include "Assets/AssetRef.h"
#include "Registry.h"

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
	TRANSFORM,
	COLOR3,
	COLOR4,
	ASSETREF,
	ENUM,
	ARRAY,
	STRUCT,
	NONE
};

/*
	these are tied to an object class/instance 
	(the value is a pointer to an already constructed variable)
	if you need properties that hold their own values use StaticProperty

	Structure:
		Name(string)
		Category(string)
		Type(PropType)
		Flags(int)
		ValueSize(size_t)
		Value(void*)
		 simple value - static bytes[ValueSize]
		 string - dynamic bytes[ValueSize(stringLength)] construct a std::string object
		 assetRef - staticBytes[ValueSize] construct a AssetRef<Asset>
		 struct - dynamic bytes[ValueSize], (string)structClassName, construct struct from structClassName using the registry 
		 array - dynamic bytes[ValueSize] construct an array by passing it a structRegistry if the element type is a struct
*/
struct DENGINE_API Property
{
	std::string m_name = "";
	std::string m_category = "";
	PropType m_Type = PropType::BOOL; 
	int m_Flags = 0;
	void* m_Value = nullptr;
	size_t m_ValueSize = 0;

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

	//reads in the name, type, category, flags and value size
	void LoadAllMetadata(const Buffer& buffer)
	{
		STARTREAD(buffer, 0)
		READSTRING(m_name);
		READ(&m_Type, sizeof(PropType));
		READSTRING(m_category);
		READ(&m_Flags, sizeof(int));
		READ(&m_ValueSize, sizeof(size_t));
	}
};

//only difference is that the Value pointer is tied to this property instance and not an external class,
//its therefor also deleted by ~StaticProperty()
struct DENGINE_API StaticProperty : public Property
{
	using Property::Property;
	
	//need a copy constructor to explicitly copy the value of this property
	StaticProperty(const StaticProperty& other);

	//copies in the value ptr with the size
	StaticProperty(void* StaticValue, size_t ValueSize)
	{
		m_Value = new byte[ValueSize];
		m_ValueSize = ValueSize;
		memcpy(m_Value, StaticValue, ValueSize);
	}

	/*
		constructs the m_value from the buffer metadata with the correct type etc
		structure value must be registered
	*/
	void FromStaticBuffer(const Buffer& buffer, StructRegistry& structRegistry);

	StaticProperty& operator=(const StaticProperty& other);
	StaticProperty(StaticProperty&& other) noexcept;

	//copy constructor from a static value (COPIES THE POINTER DATA WITH THE VALUE SIZE PASSED IN)
	void Assign(void* StaticValue, size_t ValueSize)
	{
		m_Value = new byte[ValueSize];
		m_ValueSize = ValueSize;
		memcpy(m_Value, StaticValue, ValueSize);
	}

	~StaticProperty()
	{
		if(m_Value)
			delete m_Value;
	}
};


