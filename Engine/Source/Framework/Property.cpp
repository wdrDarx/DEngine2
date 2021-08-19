#include "Property.h"
#include "StructBase.h"
#include "Array.h"
#include "Registry.h"

Buffer Property::MakeBuffer() const
{
	Buffer FinalBuffer;
	STARTWRITE(FinalBuffer, 0)

	WRITESTRING(m_name);
	WRITE(&m_Type, sizeof(PropType));
	WRITESTRING(m_category);
	WRITE(&m_Flags, sizeof(int));
	WRITE(&m_ValueSize, sizeof(size_t));

	switch (m_Type)
	{
	default:
	{
		WRITE(m_Value, m_ValueSize);
		break;
	}

	case PropType::STRING:
	{
		std::string temp = (*(std::string*)m_Value);
		WRITESTRING(temp);
		break;
	}

	case PropType::ASSETREF:
	{
		Buffer assetRefBuffer;
		AssetRef<Asset>* assetRef = (AssetRef<Asset>*)(m_Value);
		assetRef->Serialize(assetRefBuffer);
		WRITEBUFFER(assetRefBuffer);
		break;
	}

	case PropType::ARRAY:
	{
		Array<bool>* arrayRef = (Array<bool>*)(m_Value);
		Buffer arrayBuffer = arrayRef->MakeBuffer();
		WRITEBUFFER(arrayBuffer);

		break;
	}

	case PropType::STRUCT:
	{
		StructBase* Struct = (StructBase*)(m_Value);
		
		//write the class name
		std::string classname = Struct->GetClassType().Name;
		WRITESTRING(classname);

		//write the properties
		ArrayBuffer StructPropBuffer;
		Struct->SerializeProps(StructPropBuffer);
		WRITEBUFFER(StructPropBuffer.MakeBuffer());
		break;
	}
	}

	return FinalBuffer;
}

void Property::FromBuffer(const Buffer& buffer)
{
	STARTREAD(buffer, 0)
	READSTRING(m_name);
	READ(&m_Type, sizeof(PropType));
	READSTRING(m_category);
	READ(&m_Flags, sizeof(int));
	READ(&m_ValueSize, sizeof(size_t));

	switch (m_Type)
	{
	default:
	{
		READ(m_Value, m_ValueSize);
		break;
	}
	case PropType::STRING:
	{
		std::string temp;
		READSTRING(temp);
		(*(std::string*)m_Value) = temp;
		break;
	}

	case PropType::ASSETREF:
	{
		Buffer assetRefBuffer;
		READBUFFER(assetRefBuffer);
		AssetRef<Asset>* assetRef = (AssetRef<Asset>*)(m_Value);
		assetRef->Deserialize(assetRefBuffer);
		break;
	}

	case PropType::ARRAY:
	{
		Buffer arrayBuffer;
		READBUFFER(arrayBuffer);
		Array<bool>* arrayRef = (Array<bool>*)(m_Value);
		arrayRef->FromBuffer(arrayBuffer); //no need to pass in a struct registry because this array already exists as a property and has a reigstry
		break;
	}

	case PropType::STRUCT:
	{
		StructBase* StructProp = (StructBase*)(m_Value);

		//read the class name (discarded here because the Property class value is tied to an existing object of the correct type)
		std::string StructClassName;
		READSTRING(StructClassName);

		//read the properties
		Buffer temp;
		READBUFFER(temp);
		ArrayBuffer StructPropBuffer;
		StructPropBuffer.FromBuffer(temp);
		StructProp->LoadProps(StructPropBuffer);
		break;
	}
	}
}

void StaticProperty::FromStaticBuffer(const Buffer& buffer, StructRegistry& structRegistry)
{
	if(m_Value)
		delete m_Value;

	STARTREAD(buffer, 0)
	READSTRING(m_name);
	READ(&m_Type, sizeof(PropType));
	READSTRING(m_category);
	READ(&m_Flags, sizeof(int));
	READ(&m_ValueSize, sizeof(size_t));

	switch (m_Type)
	{
	default:
	{
		//simple value - allocate the size and copy over
		m_Value = new byte[m_ValueSize];
		READ(m_Value, m_ValueSize);
		break;
	}
	case PropType::STRING:
	{
		std::string temp;
		READSTRING(temp);

		//construct string
		m_Value = new std::string();
		(*(std::string*)m_Value) = temp;
		break;
	}

	case PropType::ASSETREF:
	{
		Buffer assetRefBuffer;
		READBUFFER(assetRefBuffer);

		//construct asset ref
		m_Value = new AssetRef<Asset>();
		AssetRef<Asset>* assetRef = (AssetRef<Asset>*)(m_Value);
		assetRef->Deserialize(assetRefBuffer);
		break;
	}

	case PropType::ARRAY:
	{
		Buffer arrayBuffer;
		READBUFFER(arrayBuffer);

		//construct array
		m_Value = new Array<bool>(); //the template doesnt matter so just make it bool
		Array<bool>* arrayRef = (Array<bool>*)(m_Value);
		arrayRef->FromBuffer(arrayBuffer, &structRegistry);
		break;
	}

	case PropType::STRUCT:
	{		
		//read the class name for construction with the registry
		std::string StructClassName;
		READSTRING(StructClassName);

		//construct the struct with the class name (ONLY WORKS IF ITS REGISTERED)
		m_Value = structRegistry.Make({StructClassName});

		//now that we have a pointer to a correct object we can read in the properties which are tied to pointers in the struct
		StructBase* StructProp = (StructBase*)(m_Value);
		Buffer temp;
		READBUFFER(temp);
		ArrayBuffer StructPropBuffer;
		StructPropBuffer.FromBuffer(temp);
		StructProp->LoadProps(StructPropBuffer);
		break;
	}
	}
}

StaticProperty& StaticProperty::operator=(const StaticProperty& other)
{
	m_name = other.m_name;
	m_category = other.m_category;
	m_Type = other.m_Type;
	m_Flags = other.m_Flags;

	m_Value = new byte[other.m_ValueSize];
	memcpy(m_Value, other.m_Value, other.m_ValueSize);

	m_ValueSize = other.m_ValueSize;

	//clear and define properties again for a struct 
	if (m_Type == PropType::STRUCT)
	{
		StructBase* str = (StructBase*)m_Value;
		str->GetPropertiesMutable().clear();
		str->DefineProperties();
	}
	return *this;
}

StaticProperty::StaticProperty(const StaticProperty& other)
{
	m_name = other.m_name;
	m_category = other.m_category;
	m_Type = other.m_Type;
	m_Flags = other.m_Flags;

	m_Value = new byte[other.m_ValueSize];
	memcpy(m_Value, other.m_Value, other.m_ValueSize);

	m_ValueSize = other.m_ValueSize;

	//clear and define properties again for a struct 
	if (m_Type == PropType::STRUCT)
	{
		StructBase* str = (StructBase*)m_Value;
		str->GetPropertiesMutable().clear();
		str->DefineProperties();
	}
}

StaticProperty::StaticProperty(StaticProperty&& other) noexcept
{
	m_name = other.m_name;
	m_category = other.m_category;
	m_Type = other.m_Type;
	m_Flags = other.m_Flags;

	m_Value = new byte[other.m_ValueSize];
	memmove(m_Value, other.m_Value, other.m_ValueSize);

	m_ValueSize = other.m_ValueSize;

	//clear and define properties again for a struct 
	if (m_Type == PropType::STRUCT)
	{
		StructBase* str = (StructBase*)m_Value;
		str->GetPropertiesMutable().clear();
		str->DefineProperties();
	}

	//disable the other
	other.m_Value = nullptr;
}
