#pragma once
#include "Core/Core.h"

namespace TickGroup
{
	enum TickGroup
	{
		GAME = BIT(0),
		EDITOR = BIT(1),
		PAUSED = BIT(2)
	};

}

struct DENGINE_API Tick
{
	public:
		Tick(const TickGroup::TickGroup& tg, float dt) : m_TickGroup(tg), DeltaTime(dt)
		{

		}

		const inline TickGroup::TickGroup& GetTickGroup() const
		{
			return m_TickGroup;
		}

		float DeltaTime;
	private:
		TickGroup::TickGroup m_TickGroup;
};