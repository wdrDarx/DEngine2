#pragma once
#include "Core/Core.h"

struct Event;

template<typename T>
struct Callback;


class EventDispatcher
{
public:
	
	template<typename T>
	void Dispatch(T& event)
	{
		if constexpr (std::is_base_of<Event, T>::value == false)
		{
			LogWarning("Event is not event base type");
			return;
		}

		//if event is handled then stop
		if (static_cast<Event*>(&event)->m_IsHandled)
			return;

		//using a queue because callbacks could be popped off in the for loop so we need a seperate one
		std::vector<std::function<void(T* event)>> CallbacksToCall;
		for (auto callback : m_BoundCallbacks)
		{		
			//try casting the event, if it works then call the callback			
 			T* castedEvent = dynamic_cast<T*>(callback->m_PlaceholderEvent);

			if (castedEvent)
			{
				//add callback to call queue
				if(callback->m_OnTrigger)
					CallbacksToCall.push_back(callback->m_OnTrigger);
			}
 		}

		//call the queue
		for (auto& callback : CallbacksToCall)
		{
			callback(&event);
		}
	}

	template<typename T>
	bool IsBound(Callback<T>& callback)
	{
		return std::find(m_BoundCallbacks.begin(), m_BoundCallbacks.end(), (Callback<Event>*)(&callback)) != m_BoundCallbacks.end();
	}

	template<typename T>
	void Bind(Callback<T>& callback)
	{
		callback.m_EventDispatcher = this;
		Callback<T>* ptr = static_cast<Callback<T>*>(&callback);
		m_BoundCallbacks.push_back((Callback<Event>*)ptr);
	}

	template<typename T>
	void Unbind(Callback<T>& callback)
	{
		auto it = std::find(m_BoundCallbacks.begin(), m_BoundCallbacks.end(), (Callback<Event>*)(&callback));
		if(it != m_BoundCallbacks.end())
			m_BoundCallbacks.erase(it);
	}

	std::vector<Callback<Event>*> m_BoundCallbacks;
};