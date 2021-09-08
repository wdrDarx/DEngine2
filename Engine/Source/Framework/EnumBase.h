#pragma once
#include "StructBase.h"
#include "FrameworkMacros.h"

#define BITMASK_CLASS(thisClass)  \
inline void operator|=(const EnumBase& other) { IntValue() |= other.ConstIntValue(); }  \
inline void operator|=(int other) { IntValue() |= other; }  \
inline bool operator&(const EnumBase& other) { return IntValue() & other.ConstIntValue(); }  \
inline bool operator&(int other) { return IntValue() & other; }   \
thisClass FromInt(int in) { thisClass out; out.IntValue() = in; return out; } \
inline thisClass operator|(const EnumBase& other) { return FromInt(ConstIntValue() | other.ConstIntValue()); } \
inline thisClass operator|(int other) { return FromInt(ConstIntValue() | other); }

#define _ENUM_DEF_BEGIN() void DefineEnum() override { _EnumClass* e = new _EnumClass(); m_Enum = (int*)e;
#define ENUM_DEF(EnumValue) m_StringMap[#EnumValue] = _EnumClass::EnumValue;
#define ENUM_DEF_END() }

#define ENUM_DEF_BEGIN(thisClass, EnumClassName) using _EnumClass = EnumClassName; STRUCT_CLASS_DEF(thisClass, EnumBase) STRUCT_PROPS_BEGIN() m_IsBitmask = false; STRUCT_PROPS_END() \
_EnumClass& Value() {return (_EnumClass&)*m_Enum;} \
const _EnumClass& ConstValue() const {return (const _EnumClass&)*m_Enum;} \
 _ENUM_DEF_BEGIN(EnumClassName) 

#define BITMASK_DEF_BEGIN(thisClass, EnumClassName) using _EnumClass = EnumClassName; STRUCT_CLASS_DEF(thisClass, EnumBase) STRUCT_PROPS_BEGIN() m_IsBitmask = true; STRUCT_PROPS_END() \
_EnumClass& Value() {return (_EnumClass&)*m_Enum;} \
const _EnumClass& ConstValue() const {return (const _EnumClass&)*m_Enum;} \
BITMASK_CLASS(thisClass) \
 _ENUM_DEF_BEGIN(EnumClassName) 

struct DENGINE_API EnumBase : public StructBase
{

	void operator=(int value)
	{		
		*m_Enum = value;
	}

	virtual void DefineEnum()
	{

	}

	~EnumBase()
	{
		if(m_Enum)
			delete m_Enum;
	}

	STRUCT_CLASS_DEF(EnumBase, StructBase)
	STRUCT_PROPS_BEGIN()
		DefineEnum();
		m_Properties.push_back(Property("Enum", std::string(""), PropType::INT, (void*)m_Enum, (size_t)sizeof(int), 0));
	STRUCT_PROPS_END()

	int& IntValue()
	{
		return *m_Enum;
	}

	const int& ConstIntValue() const
	{
		return *m_Enum;
	}

	bool IsBitmask() const
	{
		return m_IsBitmask;
	}

	const std::map<std::string, int>& GetEnumMap() const
	{
		return m_StringMap;
	}

	std::string ToString(int enumVal);
	int FromString(const std::string& EnumName);

	//Key = enum name, Value = pointer to int value of enum
	std::map<std::string, int> m_StringMap;
	int* m_Enum = nullptr;
	bool m_IsBitmask = false;
};
