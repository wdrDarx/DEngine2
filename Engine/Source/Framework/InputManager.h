#pragma once
#include "Core/Core.h"
#include "Event/EventDispatcher.h"
#include "Event/KeyEvent.h"
#include "Event/Callback.h"


struct GLFWwindow;


/*
	class for Input events. Can forward input events to other managers.
	One main manager needs to be tied to a window through the base private input calls
	that main manager then should relay all events to other managers (default framework relays events to the focused scene)
*/
class DENGINE_API InputManager
{
	public:
	friend class Window;

	InputManager();
	void ForwardTo(InputManager& other);
	void StopForwarding(InputManager& other);

	void BindOnKeyDown(Callback<KeyEvent>& callback);
	void BindOnKeyUp(Callback<KeyEvent>& callback);

	bool IsKeyDown(int KeyCode);

	EventDispatcher& GetInternalDispatcher()
	{
		return m_InternalDispatcher;
	}

	public:
		void GlfwKey(GLFWwindow* window, int key, int scancode, int action, int mods);

		std::vector<int> m_HeldKeys;

		EventDispatcher m_InternalDispatcher;
		Callback<KeyEvent> m_InternalKeyEvent;

		EventDispatcher m_KeyDownDispatchcer;
		EventDispatcher m_KeyUpDispatchcer;
};