#include "ObjectBase.h"
#include "Utils/Rand.h"

ObjectBase::ObjectBase(const ObjectInitializer& initializer)
{	
	//assign name (could be empty)
	m_Name = initializer.Name;

	//assign random ID
	if (initializer.Flags & ContructFlags::RANDOMID)
	{
		m_ID = { Rand::Int64() };
	}

	//start invalid
	if (initializer.Flags & ContructFlags::INVALIDATE)
	{
		m_IsValid = false;
	}

	//call the virtual constructor
	OnConstruct();
}
