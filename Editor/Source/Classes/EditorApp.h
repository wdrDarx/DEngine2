#pragma once
#include "Framework\Application.h"
#include "DEngine.h"

struct niggaEvent : public Event
{
	niggaEvent()
	{

	}

	niggaEvent(const std::string& msg) : message(msg)
	{

	}

	std::string message = "ahahahha";
};

struct niggaEvent2 : public Event
{
	niggaEvent2()
	{

	}

	niggaEvent2(const std::string& msg) : message(msg)
	{

	}

	std::string message = "uhuhuhu";
};

class EditorApp : public Application
{
public:
	EditorApp();

	void OnUpdate(const Tick& tick) override;

	EventDispacher m_EventDispatcher;

	Callback<niggaEvent> m_Callback;
	Callback<niggaEvent2> m_Callback2;
};

