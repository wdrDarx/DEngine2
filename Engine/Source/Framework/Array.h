#pragma once
#include "StructBase.h"
#include "FrameworkMacros.h"

struct DENGINE_API _ArrayInternal : public StructBase
{
	STRUCT_CLASS_DEF(_ArrayInternal, StructBase)

	virtual Buffer MakeBuffer() const = 0;
	virtual void FromBuffer(const Buffer& buffer) = 0;
};

template<typename _Type>
struct Array : public _ArrayInternal
{
	STRUCT_CLASS_DEF(Array<_Type>, _ArrayInternal)

	const _Type& Get(size_t index)
	{
		ASSERT(index < m_InternalArray.size());

		const StaticProperty& elem = m_InternalArray[index];
		return *(_Type*)(elem.m_Value);
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

	//clears the current elements and creates new ones from this buffer (THIS USES THE _TYPE TEMPLATE, DONT CALL AFTER DOING A CAST TO ANOTHER TEMPLATE)
	void FromBuffer(const Buffer& buffer) override
	{
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
			//construct the static property with the correct type before deserializing
			StaticProperty prop;
			prop.m_Value = new _Type();
			prop.m_ValueSize = sizeof(_Type);

			//now we can copy over the data safely 
			prop.FromBuffer(piece);
			m_InternalArray.push_back(prop);
		}
	}

	PropType m_ElementType = GetElementType();
	std::string m_ElementClassName = GetElementClassType().Name;
	size_t m_ElementSize = GetElementSize();
	std::vector<StaticProperty> m_InternalArray;

private:
	PropType GetElementType() const
	{
		PropType out = _TO_PROP_TYPE(_Type);
		if (out == PropType::NONE || out == PropType::ARRAY) 
			ASSERT(false); //invalid types

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