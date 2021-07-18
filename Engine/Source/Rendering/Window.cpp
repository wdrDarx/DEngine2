#include "Window.h"

Window::Window(const std::string& name, int width, int height) : m_Width(width), m_Height(height)
{
	glfwInit();
	m_Window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	SetCurrentContext();

	ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));
	glViewport(0, 0, width, height);
}

void Window::StartFrame()
{
	if (!m_Window) return;

	if (glfwWindowShouldClose(m_Window))
	{
		glfwTerminate();
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT);
}

void Window::EndFrame()
{
	if(!m_Window) return;

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}
