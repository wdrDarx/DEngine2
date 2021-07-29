#include "Property.h"
#include "StructBase.h"
#include "Array.h"

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
		arrayRef->FromBuffer(arrayBuffer);
		break;
	}

	case PropType::STRUCT:
	{
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
