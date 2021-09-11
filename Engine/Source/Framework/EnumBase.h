#pragma once
#include "StructBase.h"
#include "FrameworkMacros.h"

#define BITMASK_CLASS(thisClass)  \
inline void operator|=(const EnumBase& other) { IntValue() |= other.ConstIntValue(); }  \
inline void operator|=(int other) { IntValue() |= other; }  \
inline bool operator&(const EnumBase& other) const { return (ConstIntValue() & other.ConstIntValue()); } \
inline bool operator&(int other) const { return (ConstIntValue() & other); }  \
inline thisClass FromInt(int in) const { thisClass out; out.IntValue() = in; return out; }  \
inline thisClass operator|(const EnumBase& other) const { return FromInt(ConstIntValue() | other.ConstIntValue()); } \
inline thisClass operator|(int other) const { return FromInt(ConstIntValue() | other); }

#define _ENUM_DEF_BEGIN() void DefineEnum() override { _EnumClass e = _EnumClass(); m_Enum = (int)e; } std::map<std::string, int> GetEnumMap() override { std::map<std::string, int> _StringMap;
#define ENUM_DEF(EnumValue) _StringMap[#EnumValue] = _EnumClass::EnumValue;
#define ENUM_DEF_END() return _StringMap; }

#define ENUM_DEF_BEGIN(thisClass, EnumClassName) using _EnumClass = EnumClassName; STRUCT_CLASS_DEF(thisClass, EnumBase) STRUCT_PROPS_BEGIN() m_IsBitmask = false; STRUCT_PROPS_END() \
_EnumClass& Value() {return (_EnumClass&)m_Enum;} \
const _EnumClass& ConstValue() const {return (const _EnumClass&)m_Enum;} \
 _ENUM_DEF_BEGIN() 

#define BITMASK_DEF_BEGIN(thisClass, EnumClassName) using _EnumClass = EnumClassName; STRUCT_CLASS_DEF(thisClass, EnumBase) STRUCT_PROPS_BEGIN() m_IsBitmask = true; STRUCT_PROPS_END() \
_EnumClass& Value() {return (_EnumClass&)m_Enum;} \
const _EnumClass& ConstValue() const {return (const _EnumClass&)m_Enum;} \
BITMASK_CLASS(thisClass) \
 _ENUM_DEF_BEGIN() 

struct DENGINE_API EnumBase : public StructBase
{
	STRUCT_CLASS_DEF(EnumBase, StructBase)
	STRUCT_PROPS_BEGIN()
		m_Properties.push_back(Property("Enum", "Default", PropType::INT, &m_Enum, sizeof(int), 0));
	STRUCT_PROPS_END()

	void operator=(const int& value)
	{
		m_Enum = value;
	}

	//Key = enum name, Value = pointer to int value of enum
	virtual std::map<std::string, int> GetEnumMap()
	{
		return std::map<std::string, int>();
	}

	virtual void DefineEnum()
	{
		
	}

	int& IntValue()
	{
		return m_Enum;
	}

	int ConstIntValue() const
	{
		return m_Enum;
	}

	bool IsBitmask() const
	{
		return m_IsBitmask;
	}

	std::string ToString(int enumVal);
	int FromString(const std::string& EnumName);	

	int m_Enum = 0;
	bool m_IsBitmask = false;
};
