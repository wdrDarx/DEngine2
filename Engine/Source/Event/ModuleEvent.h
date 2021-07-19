#pragma once
#include "Event/Event.h"


enum class ModuleEventType
{
	LOADED = 0,
	UNLOADED
};

struct DENGINE_API ModuleEvent : public Event
{
	ModuleEventType m_EventType;
	std::string m_ModuleName;

	ModuleEvent()
	{

	}

	ModuleEvent(ModuleEventType type, const std::string& name) : m_EventType(type), m_ModuleName(name)
	{

	}

	const ModuleEventType& GetEventType() const
	{
		return m_EventType;
	}
};