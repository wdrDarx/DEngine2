#pragma once
#include "Core/Core.h"


class EventDispacher;

template<typename EventType>
struct DENGINE_API Callback
{
public:
	Callback()
	{
		m_PlaceholderEvent = new EventType();
	}
	
	virtual ~Callback()
	{
		delete m_PlaceholderEvent;

		if (m_EventDispacher)
		{
			m_EventDispacher->Unbind<EventType>(*(this));
		}
	}

	//assign a function (from a class) to the actual callback
	void Assign(std::function<void(EventType* event)> func)
	{
		m_OnTrigger = func;
	}

	void Clear()
	{
		m_OnTrigger = nullptr;
	}

	//called when the event is dispached to the bound object
	std::function<void(EventType* event)> m_OnTrigger;

	//reference to auto unbind when destroyed 
	EventDispacher* m_EventDispacher;

	//exists for dynamic cast checking 
	EventType* m_PlaceholderEvent;
};

