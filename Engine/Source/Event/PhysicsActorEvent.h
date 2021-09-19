#pragma once
#include "Event/Event.h"
#include "Physics/ContactListener.h"

enum class PhysicsActorEventType
{
	ONADVANCE = 0,
	ONHIT,
	ONBEGINOVERLAP,
	ONENDOVERLAP,
};

struct DENGINE_API PhysicsActorEvent : public Event
{
public:
	const PhysicsActorEventType& GetEventType() const
	{
		return EventType;
	}

	PhysicsActorEventType EventType;
	HitResult Hit;
};