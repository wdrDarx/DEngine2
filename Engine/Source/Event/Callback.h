#pragma once
#include "Core/Core.h"


class EventDispatcher;

template<typename EventType>
struct Callback
{
public:
	Callback()
	{
		m_PlaceholderEvent = new EventType();
	}
	
	~Callback()
	{
		Destroy();
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

	void Destroy()
	{
		delete m_PlaceholderEvent;

		if (m_EventDispatcher)
		{
			m_EventDispatcher->Unbind<EventType>(*(this));
		}
	}

	//called when the event is dispached to the bound object
	std::function<void(EventType* event)> m_OnTrigger;

	//reference to auto unbind when destroyed 
	EventDispatcher* m_EventDispatcher = nullptr;

	//exists for dynamic cast checking 
	EventType* m_PlaceholderEvent = nullptr;
};

