#pragma once
#include "Core/Core.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"


struct DENGINE_API RenderStats
{
	uint DrawCalls;
};

struct VertexArray;
struct IndexBuffer;
struct Shader;
class Camera;

//houses a context and provides functions for a bunch of render stuff
class DENGINE_API RenderAPI
{
	public:
		RenderAPI(GLFWwindow* windowContext);

		void SetCurrentContext(GLFWwindow* windowContext);
		void ClearCurrentContext();

		GLFWwindow* GetCurrentContext() const
		{
			//ASSERT(m_windowContext);
			return m_windowContext;
		}

		void Clear();
		void SetClearColor(const color4& color);
		void SetViewport(const vec2d& Size);

		void DrawIndexed(Shader& shader, VertexArray& vertexArray, IndexBuffer& indexBuffer, uint32_t indexCount = 0);

		const vec2d& GetViewportSize() const
		{
			return m_LastViewportSize;
		}

		bool IsContextBound() const
		{
			return m_ContextBound;
		}

		const RenderStats& GetRenderStats() const
		{
			return m_Stats;
		}

		void SetCamera(Ref<Camera> camera)
		{
			m_Camera = camera;
		}

		Ref<Camera> GetCamera()
		{
			ASSERT(m_Camera);
			return m_Camera;
		}

	private:
		RenderStats m_Stats;
		vec2d m_LastViewportSize;

		//optional
		Ref<Camera> m_Camera;

		GLFWwindow* m_windowContext = nullptr;
		bool m_ContextBound = false;
};