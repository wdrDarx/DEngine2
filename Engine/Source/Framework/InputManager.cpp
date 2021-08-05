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

			//remove from helf keys array if its there
			auto it = std::find(m_HeldKeys.begin(), m_HeldKeys.end(), Event->GetKeyCode());
			if(it != m_HeldKeys.end())
				m_HeldKeys.erase(it);
		}
	});

	GetInternalDispatcher().Bind(m_InternalKeyEvent);
}

void InputManager::ForwardTo(InputManager& other)
{
	if(!GetInternalDispatcher().IsBound(other.m_InternalKeyEvent))
		GetInternalDispatcher().Bind(other.m_InternalKeyEvent);
}

void InputManager::StopForwarding(InputManager& other)
{
	if (GetInternalDispatcher().IsBound(other.m_InternalKeyEvent))
		GetInternalDispatcher().Unbind(other.m_InternalKeyEvent);
}

void InputManager::BindOnKeyDown(Callback<KeyEvent>& callback)
{
	m_KeyDownDispatchcer.Bind(callback);
}

void InputManager::BindOnKeyUp(Callback<KeyEvent>& callback)
{
	m_KeyUpDispatchcer.Bind(callback);
}

bool InputManager::IsKeyDown(int KeyCode)
{
	return std::find(m_HeldKeys.begin(), m_HeldKeys.end(), KeyCode) != m_HeldKeys.end();
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

