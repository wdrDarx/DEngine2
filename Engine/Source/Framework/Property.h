#pragma once
#include "Core/Core.h"
#include "Serialization/Buffer.h"
#include <variant>

template < template <typename...> class base, typename derived>
struct is_base_of_template_impl
{
	template<typename... Ts>
	static constexpr std::true_type  test(const base<Ts...>*);
	static constexpr std::false_type test(...);
	using type = decltype(test(std::declval<derived*>()));
};

template < template <typename...> class base, typename derived>
using is_base_of_template = typename is_base_of_template_impl<base, derived>::type;

typedef std::variant<bool*, int*, float*, vec2d*, vec3d*> PropValue;

struct Property;
struct DStruct
{
public:
	std::vector<Property>& GetProps()
	{
		return props;
	}
	std::vector<Property> props;

	void SerializeProps(ArrayBuffer& buffer) const;
	void LoadProps(const ArrayBuffer& buffer);
};


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
	DSTRUCT
};

struct Property
{
	std::string m_name;
	PropType m_Type;
	int m_Flags;
	void* m_Value;
	uint m_ValueSize;

	Property()
	{

	}

	Property(const std::string& name, PropType type, void* value, uint valueSize, int flags = 0) : m_name(name), m_Type(type), m_Flags(flags), m_Value(value), m_ValueSize(valueSize)
	{

	}


	Buffer MakeBuffer() const
	{
		Buffer FinalBuffer;
		STARTWRITE(FinalBuffer, 0)

		WRITESTRING(m_name);
		WRITE(&m_Type, sizeof(PropType));
		WRITE(&m_Flags, sizeof(int));
		WRITE(&m_ValueSize, sizeof(uint));

		if (m_Type == PropType::STRING)
		{
			std::string temp = *(std::string*)m_Value;
			WRITESTRING(temp);
		}
// 		else
// 			if (m_Type == PropType::COMPONENT)
// 			{
// 				uint64_t local;
// 				uint64_t owner;
// 				auto ref = static_cast<ObjectComponentRef<ObjectComponent>*>(m_Value);
// 				ref->_Get(owner, local);
// 				bool localonly = ref->GetLocalOnly();
// 
// 				WRITE(&localonly, sizeof(bool));
// 				WRITE(&local, sizeof(uint64_t));
// 				WRITE(&owner, sizeof(uint64_t));
// 			}
// 			else
// 				if (m_Type == PropType::OBJECT)
// 				{
// 					uint64_t ID;
// 					static_cast<SceneObjectRef<SceneObject>*>(m_Value)->_Get(ID);
// 					WRITE(&ID, sizeof(uint64_t));
// 				}
		if (m_Type == PropType::DSTRUCT)
		{
			DStruct* Struct = (DStruct*)(m_Value);
			ArrayBuffer StructPropBuffer;
			Struct->SerializeProps(StructPropBuffer);
			WRITEBUFFER(StructPropBuffer.MakeBuffer());
		}
		else
		{
			WRITE(m_Value, m_ValueSize);
		}

		return FinalBuffer;
	}

	//use for loading other props
	void LoadNameAndType(const Buffer& buffer)
	{
		STARTREAD(buffer, 0);
		READSTRING(m_name);
		READ(&m_Type, sizeof(PropType));
	}

	//the props already exist with an address, DO NOT delete them or use "new", just cast them to their type
	void FromBuffer(const Buffer& buffer)
	{
		STARTREAD(buffer, 0)
		READSTRING(m_name);
		READ(&m_Type, sizeof(PropType));
		READ(&m_Flags, sizeof(PropType));
		READ(&m_ValueSize, sizeof(uint));

		if (m_Type == PropType::STRING)
		{
			std::string temp;
			READSTRING(temp);
			((std::string*)m_Value)->assign(temp.c_str());
		}
// 		else
// 			if (m_Type == PropType::COMPONENT)
// 			{
// 				auto ref = (ObjectComponentRef<ObjectComponent>*)(m_Value);
// 				uint64_t local;
// 				uint64_t owner;
// 				bool localonly;
// 
// 				READ(&localonly, sizeof(bool));
// 				READ(&local, sizeof(uint64_t));
// 				READ(&owner, sizeof(uint64_t));
// 
// 				ref->_Set(owner, local);
// 				ref->SetLocalOnly(localonly);
// 			}
// 			else
// 				if (m_Type == PropType::OBJECT)
// 				{
// 					auto ref = (SceneObjectRef<SceneObject>*)(m_Value);
// 					uint64_t ID;
// 					READ(&ID, sizeof(uint64_t));
// 					ref->_Set(ID);
// 				}
		if (m_Type == PropType::DSTRUCT)
		{
			auto StructProp = (DStruct*)(m_Value);
			Buffer temp;
			READBUFFER(temp);
			ArrayBuffer StructPropBuffer;
			StructPropBuffer.FromBuffer(temp);
			StructProp->LoadProps(StructPropBuffer);
		}
		//for simple data like floats, ints, vec2d etc (where the size is constant)
		else
		{
			READ(m_Value, m_ValueSize);
		}
	}
};

static uint GetTypeSize(PropType type)
{
	switch (type)
	{
	case PropType::BOOL:
	{
		return sizeof(bool);
	}
	case PropType::INT:
	{
		return sizeof(int);
	}
	case PropType::FLOAT:
	{
		return sizeof(float);
	}
	case PropType::VEC2D:
	{
		return sizeof(vec2d);
	}

	}
	return 0;
}

template<typename T>
static T* GetPropValue(const Property& prop)
{
	return static_cast<T*>(prop.m_Value);
}


