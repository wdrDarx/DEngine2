#pragma once
#include "Core/Core.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

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
		m_ContextBound = false;
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

private:

	GLFWwindow* m_Window;
	int m_Width;
	int m_Height;
	bool m_Vsync;
	bool m_ContextBound;
};

