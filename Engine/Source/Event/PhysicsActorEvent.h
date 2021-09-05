#pragma once
#include "Event/Event.h"


enum class PhysicsActorEventType
{
	ONADVANCE = 0,
};

//inheritable class to represent an event
struct PhysicsActorEvent : public Event
{
public:
	const PhysicsActorEventType& GetEventType() const
	{
		return m_EventType;
	}

	PhysicsActorEventType m_EventType;
};