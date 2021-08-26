#pragma once
#include "Core/Core.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Rendering/Shader.h"


struct RenderStats
{
	uint DrawCalls;
};

enum class InputMode
{
	UI = 0,
	GAME
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
		void DrawIndexed(IndexBuffer& indexBuffer);
		void DrawInstanced(Shader& shader, VertexArray& vertexArray, IndexBuffer& indexBuffer, uint InstanceCount = 0, uint32_t indexCount = 0);
		void DrawInstanced(IndexBuffer& indexBuffer, uint InstanceCount);

		const vec2d& GetViewportSize() const
		{
			return m_LastViewportSize;
		}

		const vec2d& GetWindowSize() const
		{
			ASSERT(GetCurrentContext());
			int width, height;
			glfwGetWindowSize(GetCurrentContext(), &width, &height);
			return {width, height};
		}

		bool IsContextBound() const
		{
			return m_ContextBound;
		}

		RenderStats& GetRenderStats()
		{
			return m_Stats;
		}

		//needs to be called manually 
		void ResetRenderStats()
		{
			m_Stats = RenderStats();
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

		//Shader cache
		std::unordered_map<std::string, Ref<Shader>> m_ShaderCache;

		GLFWwindow* m_windowContext = nullptr;
		bool m_ContextBound = false;
};