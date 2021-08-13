#include "InputManager.h"
#include "DEngine.h"

InputManager::InputManager()
{
	m_InternalKeyEvent.Assign([&](KeyEvent* Event)
	{
		if(Event->GetKeyState() == KeyState::DOWN)
		{ 
			m_KeyDownDispatchcer.Dispatch(*Event);

			//add to held keys array if not already there
			if(std::count(m_HeldKeys.begin(), m_HeldKeys.end(), Event->GetKeyCode()) < 1)
				m_HeldKeys.push_back(Event->GetKeyCode());
		}
		else
		if(Event->GetKeyState() == KeyState::UP)
		{ 
			m_KeyUpDispatchcer.Dispatch(*Event);

			//remove from held keys array if its there
			auto it = std::find(m_HeldKeys.begin(), m_HeldKeys.end(), Event->GetKeyCode());
			if(it != m_HeldKeys.end())
				m_HeldKeys.erase(it);
		}
	});
	GetInternalDispatcher().Bind(m_InternalKeyEvent);

	m_InternalMouseEvent.Assign([&](MouseEvent* Event)
	{
		if(Event->GetEventType() == MouseEventType::BUTTON)
		{ 
			if (Event->GetKeyState() == KeyState::DOWN)
			{
				m_MouseDownDispatcher.Dispatch(*Event);

				//add to held keys array if not already there
				if (std::count(m_HeldKeys.begin(), m_HeldKeys.end(), Event->GetKeyCode()) < 1)
					m_HeldKeys.push_back(Event->GetKeyCode());
			}
			else
				if (Event->GetKeyState() == KeyState::UP)
				{
					m_MouseUpDispatcher.Dispatch(*Event);

					//remove from held keys array if its there
					auto it = std::find(m_HeldKeys.begin(), m_HeldKeys.end(), Event->GetKeyCode());
					if (it != m_HeldKeys.end())
						m_HeldKeys.erase(it);
				}
		}
		else if(Event->GetEventType() == MouseEventType::MOVED)
		{
			m_MouseMoveDispatcher.Dispatch(*Event);
		}
		else if(Event->GetEventType() == MouseEventType::SCROLL)
		{
			m_MouseScrollDispatcher.Dispatch(*Event);
		}
	});
	GetInternalDispatcher().Bind(m_InternalMouseEvent);
}

void InputManager::ForwardTo(InputManager& other)
{
	if(!GetInternalDispatcher().IsBound(other.m_InternalKeyEvent))
		GetInternalDispatcher().Bind(other.m_InternalKeyEvent);

	if (!GetInternalDispatcher().IsBound(other.m_InternalMouseEvent))
		GetInternalDispatcher().Bind(other.m_InternalMouseEvent);
}

void InputManager::StopForwarding(InputManager& other)
{
	if (GetInternalDispatcher().IsBound(other.m_InternalKeyEvent))
	{ 
		GetInternalDispatcher().Unbind(other.m_InternalKeyEvent);
	}

	if (GetInternalDispatcher().IsBound(other.m_InternalMouseEvent))
	{
		GetInternalDispatcher().Unbind(other.m_InternalMouseEvent);
	}
}

void InputManager::BindOnKeyDown(Callback<KeyEvent>& callback)
{
	m_KeyDownDispatchcer.Bind(callback);
}

void InputManager::BindOnKeyUp(Callback<KeyEvent>& callback)
{
	m_KeyUpDispatchcer.Bind(callback);
}

void InputManager::BindOnMouseMove(Callback<MouseEvent>& callback)
{
	m_MouseMoveDispatcher.Bind(callback);
}

void InputManager::BindOnMouseScroll(Callback<MouseEvent>& callback)
{
	m_MouseScrollDispatcher.Bind(callback);
}

void InputManager::BindOnMouseDown(Callback<MouseEvent>& callback)
{
	m_MouseDownDispatcher.Bind(callback);
}

void InputManager::BindOnMouseUp(Callback<MouseEvent>& callback)
{
	m_MouseUpDispatcher.Bind(callback);
}

bool InputManager::IsKeyDown(int KeyCode)
{
	return std::find(m_HeldKeys.begin(), m_HeldKeys.end(), KeyCode) != m_HeldKeys.end();
}

void InputManager::ClearInput()
{
	m_HeldKeys.clear();
}

void InputManager::GlfwKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	KeyEvent event;
	event.m_KeyCode = key;
	switch (action)
	{
		case GLFW_PRESS:
		{
			event.m_KeyState = KeyState::DOWN;
			break;
		}

		case GLFW_RELEASE:
		{
			event.m_KeyState = KeyState::UP;
			break;
		}
	}

	GetInternalDispatcher().Dispatch(event);
}

void InputManager::GlfwMouseKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	MouseEvent event;
	event.m_KeyCode = key;
	event.m_EventType = MouseEventType::BUTTON;
	switch (action)
	{
		case GLFW_PRESS:
		{
			event.m_KeyState = KeyState::DOWN;
			break;
		}

		case GLFW_RELEASE:
		{
			event.m_KeyState = KeyState::UP;
			break;
		}
	}

	GetInternalDispatcher().Dispatch(event);
}

void InputManager::GlfwMouseMove(GLFWwindow* window, const vec2d& vector)
{
	MouseEvent event;
	event.m_EventType = MouseEventType::MOVED;
	event.m_MoveVector = vector;
	LogTemp("Mouse Delta : " + Log::string(vector));

	GetInternalDispatcher().Dispatch(event);
}

void InputManager::GlfwMouseScroll(GLFWwindow* window, const ScrollDir& dir)
{
	MouseEvent event;
	event.m_EventType = MouseEventType::SCROLL;
	event.m_ScrollDir = dir;

	GetInternalDispatcher().Dispatch(event);
}

