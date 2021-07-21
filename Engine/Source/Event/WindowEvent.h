#pragma once
#include "Event/Event.h"


enum class WindowEventType
{
	CLOSED = 0,
};

//inheritable class to represent an event
struct WindowEvent : public Event
{
public:

	const WindowEventType& GetEventType() const
	{
		return m_EventType;
	}

	WindowEventType m_EventType;
};