#pragma once
#include "Core/Core.h"
#include "Event/EventDispatcher.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
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

	void BindOnMouseMove(Callback<MouseEvent>& callback);
	void BindOnMouseScroll(Callback<MouseEvent>& callback);
	void BindOnMouseDown(Callback<MouseEvent>& callback);
	void BindOnMouseUp(Callback<MouseEvent>& callback);

	bool IsKeyDown(int KeyCode);

	void ClearInput();

	const vec2d& GetMouseDelta() const
	{
		return m_MouseDelta;
	}

	EventDispatcher& GetInternalDispatcher()
	{
		return m_InternalDispatcher;
	}

	public:
		void GlfwKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		void GlfwMouseKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		void GlfwMouseMove(GLFWwindow* window, const vec2d& vector);
		void GlfwMouseScroll(GLFWwindow* window, const ScrollDir& dir);

		std::vector<int> m_HeldKeys;
		vec2d m_MouseDelta = {0,0};

		EventDispatcher m_InternalDispatcher;

		Callback<KeyEvent> m_InternalKeyEvent;
		Callback<MouseEvent> m_InternalMouseEvent;

		EventDispatcher m_KeyDownDispatchcer;
		EventDispatcher m_KeyUpDispatchcer;

		EventDispatcher m_MouseMoveDispatcher;
		EventDispatcher m_MouseScrollDispatcher;
		EventDispatcher m_MouseDownDispatcher;
		EventDispatcher m_MouseUpDispatcher;

};