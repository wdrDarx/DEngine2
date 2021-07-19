#pragma once
#include "Core/Core.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Event/EventDispatcher.h"

class DENGINE_API Window
{
public:
	Window(const std::string& name, int width, int height );

	//call to start rendering a frame
	void StartFrame();

	//call to finish the frame
	void EndFrame();

	void SetVsync(bool vsync)
	{
		m_Vsync = vsync;
		glfwSwapInterval(vsync);
	}

	void SetCurrentContext()
	{
		glfwMakeContextCurrent(m_Window);
		m_ContextBound = true;
	}

	void ClearCurrentContext()
	{
		glfwMakeContextCurrent(nullptr);
		m_ContextBound = false;
	}

	bool isContextBound() const
	{
		return m_ContextBound;
	}

	int GetWidth() const
	{
		return m_Width;
	}

	int GetHeight() const
	{
		return m_Height;
	}

	GLFWwindow* GetGlfwWindow() const
	{
		return m_Window;
	}

	EventDispatcher& GetEventDispatcher() 
	{
		return m_EventDispatcher;
	}

private:

	//calls window events
	EventDispatcher m_EventDispatcher;

	GLFWwindow* m_Window;
	int m_Width = 0;
	int m_Height = 0;
	bool m_Vsync = false;
	bool m_ContextBound = false;
};

