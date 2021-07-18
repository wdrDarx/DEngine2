#pragma once
#include "Core/Core.h"
#include "Framework/Tick.h"

//unique ID
struct UID
{
	uint64 ID;
};

/* 
	Simple class that has a property system, serialization interface,
	a unique assignable id and an Event interface
*/
class ObjectBase
{
public:
	ObjectBase()
	{

	}

	virtual void OnBeginPlay()
	{

	}

	virtual void OnUpdate(const Tick& tick)
	{

	}

	const UID& GetID() const
	{
		return m_ID;
	}

	void SetID(const UID& id)
	{
		m_ID = id;
	}


private:
		UID m_ID;
};