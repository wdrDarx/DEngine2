#pragma once
#include "Event/Event.h"
#include "Event/KeyEvent.h"

enum class MouseEventType
{
	BUTTON = 0,
	SCROLL,
	MOVED
};

enum class ScrollDir
{
	UP = 0,
	DOWN
};

struct MouseEvent : public Event
{
public:
	int m_KeyCode;
	ScrollDir m_ScrollDir;
	KeyState m_KeyState;
	MouseEventType m_EventType;
	vec2d m_MoveVector;

	const ScrollDir& GetScrollDir() const
	{
		return m_ScrollDir;
	}

	const vec2d& GetMoveVector() const
	{
		return m_MoveVector;
	}

	const KeyState& GetKeyState() const
	{
		return m_KeyState;
	}

	int GetKeyCode() const
	{
		return m_KeyCode;
	}
	
	const MouseEventType& GetEventType() const
	{
		return m_EventType;
	}

};