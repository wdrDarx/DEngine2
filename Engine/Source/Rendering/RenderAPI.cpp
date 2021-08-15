#include "RenderAPI.h"
#include "VertexArray.h"
#include "Indexbuffer.h"
#include "Shader.h"


void OpenGLMessageCallback(
	unsigned source,
	unsigned type,
	unsigned id,
	unsigned severity,
	int length,
	const char* message,
	const void* userParam)
{
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         LogError(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       LogWarning(message); return;
		case GL_DEBUG_SEVERITY_LOW:          LogWarning(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: LogTemp(message); return;
	}

	ASSERT(false); //severity?
}

RenderAPI::RenderAPI(GLFWwindow* windowContext) : m_windowContext(windowContext)
{
	ASSERT(windowContext); // this class requires a context
	SetCurrentContext(windowContext);

	//load GLAD
	ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

	//opengl debug messaging 
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(OpenGLMessageCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
}

void RenderAPI::SetCurrentContext(GLFWwindow* windowContext)
{
	glfwMakeContextCurrent(windowContext);
	m_windowContext = windowContext;
	m_ContextBound = true;
}

void RenderAPI::ClearCurrentContext()
{
	glfwMakeContextCurrent(nullptr);
	m_windowContext = nullptr;
	m_ContextBound = false;
}
void RenderAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_Stats = RenderStats();
}

void RenderAPI::SetClearColor(const color4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void RenderAPI::SetViewport(const vec2d& Size)
{
	glViewport(0,0, (GLsizei)Size.x, (GLsizei)Size.y);
	m_LastViewportSize = Size;
}

void RenderAPI::DrawIndexed(Shader& shader, VertexArray& vertexArray, IndexBuffer& indexBuffer, uint32_t indexCount /*= 0*/)
{
	uint32_t count = indexCount ? indexCount : indexBuffer.GetCount();
	shader.Bind();
	vertexArray.Bind();
	indexBuffer.Bind();

	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

	shader.Unbind();
	vertexArray.Unbind();
	indexBuffer.Unbind();

	//record draw calls
	m_Stats.DrawCalls++;
}

void RenderAPI::DrawInstanced(Shader& shader, VertexArray& vertexArray, IndexBuffer& indexBuffer, uint InstanceCount /*= 0*/, uint32_t indexCount /*= 0*/)
{
	uint32_t count = indexCount ? indexCount : indexBuffer.GetCount();
	shader.Bind();
	vertexArray.Bind();
	indexBuffer.Bind();

	glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, InstanceCount);

	shader.Unbind();
	vertexArray.Unbind();
	indexBuffer.Unbind();

	//record draw calls
	m_Stats.DrawCalls++;
}

void RenderAPI::SetShowCursor(bool show)
{
	glfwSetInputMode(GetCurrentContext(), GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void RenderAPI::SetCursorPos(const vec2d& pos)
{
	//invert for glfw
	glfwSetCursorPos(GetCurrentContext(), pos.x , GetWindowSize().y - pos.y);
}

void RenderAPI::SetUseRawMouseInput(bool use)
{
	glfwSetInputMode(GetCurrentContext(), GLFW_RAW_MOUSE_MOTION, use);
}

void RenderAPI::AddShaderToCache(Ref<Shader> shader, const std::string& shaderName)
{
	m_ShaderCache[shaderName] = shader;
}

void RenderAPI::ReloadShader(const std::string& shaderName)
{
	for (auto& pair : m_ShaderCache)
	{
		if (pair.first == shaderName)
		{ 
			pair.second = MakeRef<Shader>(pair.second->m_Filepath);
			break;
		}
	}
}

bool RenderAPI::IsShaderInCache(const std::string& shaderName)
{
	bool found = false;
	for (auto& pair : m_ShaderCache)
	{
		if (pair.first == shaderName)
		{
			return true;
		}
	}

	return false;
}

