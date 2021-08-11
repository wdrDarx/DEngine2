#include "Window.h"
#include "Event/WindowEvent.h"
#include "Event/Callback.h"

Window::Window(const std::string& name, int width, int height)
{
	glfwInit();
	m_Window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

	//create render api
	m_RenderAPI = MakeRef<RenderAPI>(m_Window);
	SetCurrentContext();

	//set viewport
	GetRenderAPI()->SetViewport({width, height});

	//point to this window
	glfwSetWindowUserPointer(GetRenderAPI()->GetCurrentContext(), (void*)this);

	//bind resize callback
	glfwSetWindowSizeCallback(GetRenderAPI()->GetCurrentContext(), [](GLFWwindow* window, int width, int height)
	{
		StaticCast<Window>(glfwGetWindowUserPointer(window))->OnWindowResize(window, width, height);
	});

	//bind key input callback to the input manager
	glfwSetKeyCallback(GetRenderAPI()->GetCurrentContext(), [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		StaticCast<Window>(glfwGetWindowUserPointer(window))->GetInputManager().GlfwKey(window, key, scancode, action, mods);
	});	
}

void Window::StartFrame()
{
	if (!GetRenderAPI()->GetCurrentContext()) return;

	if (glfwWindowShouldClose(GetRenderAPI()->GetCurrentContext()))
	{
		WindowEvent event;
 		event.m_EventType = WindowEventType::CLOSED;
 		m_EventDispatcher.Dispatch(event);

		glfwDestroyWindow(GetRenderAPI()->GetCurrentContext());
		ClearCurrentContext();
		return;
	}

	GetRenderAPI()->SetClearColor({0,0,0,0});
	GetRenderAPI()->Clear();
}

void Window::EndFrame()
{
	if(!GetRenderAPI()->GetCurrentContext()) return;

	glfwSwapBuffers(GetRenderAPI()->GetCurrentContext());
	glfwPollEvents();
}

void Window::OnWindowResize(GLFWwindow* window, int width, int height)
{
	//dispatch resize event
	WindowEvent event;
	event.m_EventType = WindowEventType::RESIZED;
	event.m_NewSize = {width, height};
	m_EventDispatcher.Dispatch(event);

	GetRenderAPI()->SetViewport({width, height});
}

void Window::SetCurrentContext()
{
	GetRenderAPI()->SetCurrentContext(m_Window);
}

void Window::ClearCurrentContext()
{
	GetRenderAPI()->ClearCurrentContext();
}

bool Window::isContextBound()
{
	return GetRenderAPI()->IsContextBound();
}

int Window::GetWidth() const
{
	return (int)GetRenderAPI()->GetViewportSize().x;
}

int Window::GetHeight() const
{
	return (int)GetRenderAPI()->GetViewportSize().y;
}
