#pragma once
#include "Core/Core.h"
#include "Event/EventDispatcher.h"
#include "RenderAPI.h"

//houses 
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

	void OnWindowResize(GLFWwindow* window, int width, int height);

	void SetCurrentContext();

	void ClearCurrentContext();

	bool isContextBound();

	int GetWidth() const;

	int GetHeight() const;

	GLFWwindow* GetGlfwWindow() const
	{
		return m_Window;
	}

	EventDispatcher& GetEventDispatcher() 
	{
		return m_EventDispatcher;
	}

	Ref<RenderAPI> GetRenderAPI() const
	{
		return m_RenderAPI;
	}

private:

	//calls window events
	EventDispatcher m_EventDispatcher;

	GLFWwindow* m_Window;
	Ref<RenderAPI> m_RenderAPI;

	bool m_Vsync = false;
	bool m_ContextBound = false;
};

