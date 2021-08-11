#pragma once
#include "Core/Core.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Rendering/Shader.h"


struct DENGINE_API RenderStats
{
	uint DrawCalls;
};

struct VertexArray;
struct IndexBuffer;
struct FrameBuffer;
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

		RenderStats& GetRenderStats()
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

		void SetActiveFramebuffer(Ref<FrameBuffer> framebuffer)
		{
			m_ActiveFrameBuffer = framebuffer;
		}

		Ref<FrameBuffer> GetActiveFrameBuffer()
		{
			return m_ActiveFrameBuffer;
		}

		void AddShaderToCache(Ref<Shader> shader, const std::string& shaderName);

		Ref<Shader> GetShaderFromCache(const std::string& shaderName)
		{
			return m_ShaderCache[shaderName];
		}

		std::vector<std::pair<std::string, Ref<Shader>>> GetAllShadersInCache() const
		{
			std::vector<std::pair<std::string, Ref<Shader>>> temp;
			for (auto& pair : m_ShaderCache)
			{
				temp.push_back(pair);
			}

			return temp;
		}

		void ReloadShader(const std::string& shaderName);
		bool IsShaderInCache(const std::string& shaderName);

	private:
		RenderStats m_Stats;
		vec2d m_LastViewportSize;

		//optional
		Ref<Camera> m_Camera;

		//optional
		Ref<FrameBuffer> m_ActiveFrameBuffer;

		//Shader cache
		std::unordered_map<std::string, Ref<Shader>> m_ShaderCache;

		GLFWwindow* m_windowContext = nullptr;
		bool m_ContextBound = false;
};