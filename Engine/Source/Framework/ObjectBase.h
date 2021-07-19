#pragma once
#include "Core/Core.h"
#include "Framework/Tick.h"
#include "Framework/Property.h"
#include "Serialization/Buffer.h"


//simple definitions given the name of the class and its super class
#define OBJECT_CLASS_DEF(class, superclass) using Super = superclass; \
ClassType GetClassType() const override { return typeid(this); }; \
using superclass::superclass; \
class(const ObjectInitializer& initializer) : superclass(initializer) {};

//get static properties of a class by constructing an instance of it
#define OBJECT_STATIC_PROPERTIES(ObjectClass) [&]() -> std::vector<Property> { ObjectClass obj; obj.DefineProperties(); return obj.GetProperties(); }();

#define _PROP_MEMBER_NAME m_Properties

//Simple prop additions macro
#define PROPDEF_FLAGS(x, flags) { int _flags = flags; \
if(typeid(x) == typeid(bool)) {_PROP_MEMBER_NAME.push_back(Property(#x, PropType::BOOL,			 &x, sizeof(bool),  _flags)); } else \
if(typeid(x) == typeid(int)) {_PROP_MEMBER_NAME.push_back(Property(#x, PropType::INT,			 &x, sizeof(int),   _flags)); } else \
if(typeid(x) == typeid(float)) {_PROP_MEMBER_NAME.push_back(Property(#x, PropType::FLOAT,		 &x, sizeof(float), _flags)); } else \
if(typeid(x) == typeid(std::string)) {_PROP_MEMBER_NAME.push_back(Property(#x, PropType::STRING, &x, 256,           _flags)); } else \
if(typeid(x) == typeid(vec2d)) {_PROP_MEMBER_NAME.push_back(Property(#x, PropType::VEC2D,	     &x, sizeof(vec2d), _flags)); } else \
if(typeid(x) == typeid(vec3d)) {_PROP_MEMBER_NAME.push_back(Property(#x, PropType::VEC3D,		 &x, sizeof(vec3d), _flags)); } else \
if(std::is_base_of<DStruct, decltype(x)>::value) {_PROP_MEMBER_NAME.push_back(Property(#x, PropType::DSTRUCT, &x, sizeof(x), _flags)); } \
}

//TODO give all "DStructs" a function that returns their serializable size based on their properties

#define PROPDEF(x, flags) PROPDEF_FLAGS(x, flags)

#define OBJECT_PROPS_BEGIN() void DefineProperties() override { 
#define OBJECT_PROPS_END() }

namespace ContructFlags
{
	enum DENGINE_API ContructFlags
	{
		//assign a random ID on construct
		RANDOMID = BIT(0),

		//start invalid
		INVALIDATE = BIT(1)
	};
}

//unique ID (0 means invalid)
struct DENGINE_API UID
{
	uint64 ID = 0;
};

//contains initializer values for an object
struct DENGINE_API ObjectInitializer
{
	//TODO make name non-copy
	std::string Name;
	int Flags;

	ObjectInitializer(const std::string& name, int flags) : Name(name), Flags(flags)
	{

	}

	ObjectInitializer(ContructFlags::ContructFlags flags) : Flags(flags)
	{

	}

	ObjectInitializer()
	{

	}
};

struct ClassType 
{
	std::type_index typeIndex;
	std::string Name;

	static std::string GetFriendlyTypeName(const std::type_index& index)
	{
		std::string base = index.name();

		return base;
	}

	ClassType(const std::type_index& index) : typeIndex(index)
	{
		Name = GetFriendlyTypeName(index);
	}
};

/* 
	Simple class that has a property system, serialization interface,
	a unique assignable id and an Event interface.

	has an empty contrutor and an explicit initializer construtor
	that needs to be called manually - Initialize(const ObjectInitializer& initializer)

	also has a constructor that will call initializer - ObjectBase(const ObjectInitializer& initializer)
*/

class _placeholder
{
protected:
	virtual uint Serialize(Buffer& buffer)
	{
		return 0;
	}

	virtual uint Deserialize(const Buffer& buffer)
	{
		return 0;
	}
};
class DENGINE_API ObjectBase : public _placeholder
{
public:
	using Super = _placeholder;
	//empty constructor
	ObjectBase()
	{

	}

	//Calls initialize 
	ObjectBase(const ObjectInitializer& initializer)
	{
		Initialize(initializer);
	}


	//basically the actual constructor
	void Initialize(const ObjectInitializer& initializer);


	//this is manually overriden on all object classes with a macro
	virtual ClassType GetClassType() const
	{
		return typeid(this);
	}

	virtual void OnConstruct()
	{

	}

	virtual void OnBeginPlay()
	{

	}

	virtual void OnUpdate(const Tick& tick)
	{

	}

	//use to define props here with PROPDEF(x)
	virtual void DefineProperties()
	{

	}

	bool IsValid() const
	{
		return m_IsValid;
	}

	void Invalidate()
	{
		m_IsValid = false;
	}

	const UID& GetID() const
	{
		return m_ID;
	}

	void SetID(const UID& id)
	{
		m_ID = id;
	}

	const std::string& GetName() const
	{
		return m_Name;
	}
	
	void SetName(const std::string& name)
	{
		m_Name = name;
	}

	const std::vector<Property>& GetProperties() const
	{
		return m_Properties;
	}

	//automatically serializes props, can be customized and overriden
	virtual uint Serialize(Buffer& buffer);

	//automatically Deserializes props, can be customized and overriden
	virtual uint Deserialize(const Buffer& buffer);

	//Saves all current properties as a buffer array outputed as a single buffer
	Buffer GeneratePropBuffer() const;

	//Loads all found props in a buffer 
	void LoadPropsFromBuffer(const Buffer& buffer);

	/*
		View the default properties of an object class (FOR REFLECTION ONLY)
		this creates an instance of the object without calling the OnConstruct() function
 	*/
	template<class ObjectClass>
	inline static const std::vector<Property>& GetStaticProperties()
	{
		bool valid = std::is_base_of<ObjectBase, ObjectClass>::value;
		ASSERT(valid);

		ObjectClass obj;
	    obj.DefineProperties();

		return obj.GetProperties();
	}

	void MarkDelete()
	{
		m_MarkDelete = true;
	}

	bool IsMarkedForDeletion() const
	{
		return m_MarkDelete;
	}

	const std::string& GetAssignedModuleName() const
	{
		return m_AssignedModuleName;
	}

	void SetAssignedModuleName(const std::string& name)
	{
		m_AssignedModuleName = name;
	}

protected:

	//array of properties (used for serialization and reflection)
	std::vector<Property> m_Properties;

private:

	//used for invalidating objects for events and such
	bool m_IsValid = true;

	//flag used for deleting this object next tick or something
	bool m_MarkDelete = false;

	//optional name of associated module of this object (used mainly for auto deleting objects when a module gets unloaded)
	std::string m_AssignedModuleName;

	//unique assignable ID
	UID m_ID;

	//name of the object (could be empty)
	std::string m_Name = "";
};