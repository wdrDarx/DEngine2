#pragma once
#include "Event/Event.h"


enum class KeyState
{
	DOWN = 0,
	UP,
	CLICKED
};

struct KeyEvent : public Event
{
public:
	int m_KeyCode;
	KeyState m_KeyState;

	const KeyState& GetKeyState() const
	{
		return m_KeyState;
	}

	int GetKeyCode() const
	{
		return m_KeyCode;
	}
};