#pragma once
#include "Event/Event.h"


enum class ApplicationEventType
{
	OBJECTFINSIHUPDATE = 0,
};

//inheritable class to represent an event
struct ApplicationEvent : public Event
{
public:
	const ApplicationEventType& GetEventType() const
	{
		return m_EventType;
	}

	ApplicationEventType m_EventType;
};