#pragma once
#include "Core/Core.h"

struct Event;

template<typename T>
struct Callback;


class DENGINE_API EventDispatcher
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

		for (auto it = m_BoundCallbacks.begin(); it != m_BoundCallbacks.end(); it++)
		{
			//if event is handled then stop
			if(static_cast<Event*>(&event)->m_IsHandled)
				return;

			//try casting the event, if it works then call the callback
			auto callback = (*it);
 			T* castedEvent = dynamic_cast<T*>(callback->m_PlaceholderEvent);

			if (castedEvent)
			{
				//call the callback
				if(callback->m_OnTrigger)
					callback->m_OnTrigger(&event);
			}
 		}
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
		m_BoundCallbacks.erase(std::remove(m_BoundCallbacks.begin(), m_BoundCallbacks.end(), (Callback<Event>*)(&callback)));
	}

	std::vector<Callback<Event>*> m_BoundCallbacks;
};