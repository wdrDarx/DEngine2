#pragma once
#include "Event/Event.h"


enum class WindowEventType
{
	CLOSED = 0,
	RESIZED
};

//inheritable class to represent an event
struct WindowEvent : public Event
{
public:
	vec2d m_NewSize = {0,0};

	const vec2d& GetNewSize() const
	{
		return m_NewSize;
	}

	const WindowEventType& GetEventType() const
	{
		return m_EventType;
	}

	WindowEventType m_EventType;
};