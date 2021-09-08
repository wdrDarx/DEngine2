#pragma once
#include "StructBase.h"
#include "FrameworkMacros.h"
#include "EnumBase.h"

struct DENGINE_API _ArrayInternal : public StructBase
{
	STRUCT_CLASS_DEF(_ArrayInternal, StructBase)

	virtual Buffer MakeBuffer() const = 0;
	virtual void FromBuffer(const Buffer& buffer, StructRegistry* structRegistry = nullptr) = 0;
};

template<typename _Type>
struct DENGINE_API Array : public _ArrayInternal
{
	STRUCT_CLASS_DEF(Array<_Type>, _ArrayInternal)

	const _Type& Get(size_t index)
	{
		if (index >= m_InternalArray.size())
		{
			LogError("Array index exceeded!");
			index = m_InternalArray.size() - 1;
		}

		const StaticProperty& elem = m_InternalArray[index];
		return *(_Type*)(elem.m_Value);
	}

	size_t Size() const
	{
		return m_InternalArray.size();
	}

	void Add(const _Type& elem)
	{
		StaticProperty newElem((void*)&elem, sizeof(elem));
		newElem.m_Type = m_ElementType;
		m_InternalArray.push_back(newElem);
	}

	const _Type& operator[](size_t index)
	{
		return Get(index);
	}

	//makes a buffer from the current elements and the element type
	Buffer MakeBuffer() const override
	{
		Buffer finalBuffer;
		STARTWRITE(finalBuffer, 0);
		WRITE(&m_ElementType, sizeof(PropType));
		WRITESTRING(m_ElementClassName);

		ArrayBuffer out;
		for (auto& prop : m_InternalArray)
		{
			out.AddPiece(prop.MakeBuffer());
		}
		WRITEBUFFER(out.MakeBuffer());

		return finalBuffer;
	}

	//clears the current elements and creates new ones from this buffer, requires the struct registry
	void FromBuffer(const Buffer& buffer, StructRegistry* structRegistry = nullptr) override
	{
		StructRegistry* UsedRegistrty = structRegistry ? structRegistry : m_StructRegistry;
		ASSERT(UsedRegistrty); //MUST HAVE A VALID REGISTRY FOR THIS TO WORK

		STARTREAD(buffer, 0);
		READ(&m_ElementType, sizeof(PropType));
		READSTRING(m_ElementClassName);

		Buffer arrayBuffer;
		READBUFFER(arrayBuffer);

		ArrayBuffer in;
		in.FromBuffer(arrayBuffer);
		m_InternalArray.clear();

		for (auto& piece : in.m_DataPieces)
		{
			//allocate the value for the static property with the correct size before deserializing
			StaticProperty prop;
			m_InternalArray.push_back(prop);

			//get that copied static property and make it load the data
			m_InternalArray[m_InternalArray.size() - 1].FromStaticBuffer(piece, *UsedRegistrty);
		}
	}

	PropType m_ElementType = GetElementType();
	std::string m_ElementClassName = GetElementClassType().Name;
	size_t m_ElementSize = GetElementSize();
	std::vector<StaticProperty> m_InternalArray;

	//set with the macro PROPDEF_ARRAY
	StructRegistry* m_StructRegistry;
private:

	PropType GetElementType() const
	{
		PropType out = _TO_PROP_TYPE(_Type);
		if (out == PropType::NONE || out == PropType::ARRAY || out == PropType::ASSETREF) 
			LogError("Array cant be type " + STRING((int)out));

		return out;
	}

	//(THIS USES THE _TYPE TEMPLATE, DONT CALL AFTER DOING A CAST TO ANOTHER TEMPLATE)
	ClassType GetElementClassType()
	{
		return ClassType(typeid(_Type));
	}

	//(THIS USES THE _TYPE TEMPLATE, DONT CALL AFTER DOING A CAST TO ANOTHER TEMPLATE)
	size_t GetElementSize()
	{
		return sizeof(_Type);
	}
};