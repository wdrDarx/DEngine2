#if 0
#pragma once
#include "Core/Core.h"
#include "Core/ECS/Object.h"
#include "Core/Application.h"

class TimerObject : public Object
{
public:

	TimerObject(Application* app, const float& Delay,const bool& loop, const std::function<void()>& Callback) : Object(app), m_Delay(Delay), m_Loop(loop), m_Callback(Callback)
	{
		RegisterObject<TimerObject>("Timer");
	}

	void OnUpdate(const float& DeltaSeconds) override
	{
		m_CurrentTime += DeltaSeconds;
		if (m_CurrentTime >= m_Delay)
		{
			m_Callback();
			if(!m_Loop)
				GetApplication()->DestroyObject(this);
			else
				m_CurrentTime = 0.f;
		}
	}

protected:
	float m_CurrentTime;
	float m_Delay;
	bool m_Loop;
	std::function<void()> m_Callback;
};

class Timer
{
public:
	static void Create(Application* app, const float& Delay, const std::function<void()>& Callback, const bool& loop = false)
	{
		app->CreateObject<TimerObject>(Delay, loop, Callback);
	}
};
#endif

