#pragma once
#include "Core/Core.h"
#include "Framework/Tick.h"


//simple definitions given the name of the class and its super class
#define OBJECT_CLASS_DEF(class, superclass) using Super = superclass; using superclass::superclass;

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
	ContructFlags::ContructFlags Flags;

	ObjectInitializer(const std::string& name, ContructFlags::ContructFlags flags) : Name(name), Flags(flags)
	{

	}

	ObjectInitializer(ContructFlags::ContructFlags flags) : Flags(flags)
	{

	}

	ObjectInitializer()
	{

	}
};

/* 
	Simple class that has a property system, serialization interface,
	a unique assignable id and an Event interface.

	has an empty contrutor and an explicit initializer construtor
	that needs to be called manually - Initialize(const ObjectInitializer& initializer)

	also has a constructor that will call initializer - ObjectBase(const ObjectInitializer& initializer)
*/
class DENGINE_API ObjectBase
{
public:

	//empty constructor
	ObjectBase()
	{

	}

	//Calls initialize 
	ObjectBase(const ObjectInitializer& initializer);


	//basically the actual constructor
	void Initialize(const ObjectInitializer& initializer);

	virtual void OnConstruct()
	{

	}

	virtual void OnBeginPlay()
	{

	}

	virtual void OnUpdate(const Tick& tick)
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

private:

	//used for invalidating objects for events and such
	bool m_IsValid;

	//unique assignable ID
	UID m_ID;

	//name of the object (could be empty)
	std::string m_Name;
};