#include "QuadRenderer.h"

void QuadRenderer::OnConstruct()
{
	Super::OnConstruct();

	m_QuadShader = MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\QuadShader.shader");
	m_VertexArray = MakeRef<VertexArray>();
	m_IndexBuffer = MakeRef<IndexBuffer>();
	m_VertexBuffer = MakeRef<VertexBuffer>();
	m_VertexBufferLayout = MakeRef<VertexBufferLayout>();


	m_VertexBufferLayout->Push<float>(3); //position
	m_VertexBufferLayout->Push<float>(2); //texture coords
	m_VertexBufferLayout->Push<float>(1); //texture index
	m_VertexBufferLayout->Push<float>(4); //color

	m_VertexBufferLayout->Push<float>(4); //matrix
	m_VertexBufferLayout->Push<float>(4);
	m_VertexBufferLayout->Push<float>(4);
	m_VertexBufferLayout->Push<float>(4);

	m_VertexArray->Addbuffer(*m_VertexBuffer, *m_VertexBufferLayout);
}

void QuadRenderer::BeginFrame()
{
	Super::BeginFrame();
}

void QuadRenderer::EndFrame()
{
	Super::EndFrame();

	vec2d size = GetScene()->GetRenderAPI()->GetViewportSize();
	glm::mat4 CameraTransform = glm::translate(glm::mat4(1.0f), {0,0, -10});
	m_ViewProjectionMatrix = glm::perspective(glm::radians(90.f), size.x / size.y, 0.01f, 10000.f) * glm::inverse(CameraTransform);

	m_ViewProjectionMatrix = glm::ortho(-size.x, size.x, -size.y, size.y, -1.0f, 1.0f);

	m_VertexBuffer->SetNewData(m_Verticies.data(), sizeof(Vertex) * m_Verticies.size());
	std::vector<uint> indexbuffer;
	uint quadCount = m_Verticies.size() / 4;
	for(uint i = 0; i < quadCount; i++)
	{ 		
		indexbuffer.insert(indexbuffer.end(), m_QuadIndecies.begin(), m_QuadIndecies.end());
	}
	m_IndexBuffer->SetData(indexbuffer.data(), indexbuffer.size());

	m_QuadShader->Bind();
	m_QuadShader->SetUniformMat4f("u_ViewProjectionMatrix", m_ViewProjectionMatrix);
	GetScene()->GetRenderAPI()->DrawIndexed(*m_QuadShader, *m_VertexArray, *m_IndexBuffer);

	m_Verticies.clear();
}

void QuadRenderer::DrawQuad2D(const vec2d& pos, const vec2d& scale, const color4& color)
{
	glm::mat4 Transform = glm::translate(glm::mat4(1.0f), {pos.x, pos.y, 0.f}) * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.f});

	Vertex v1 
	{
		{-0.5f, -0.5f, 0.f},
		{0,0},
		0.f,
		color,
		Transform
	};
		
	Vertex v2
	{
		{-0.5f, 0.5f, 0.f},
		{0,0},
		0.f,
		color,
		Transform
	};

	Vertex v3
	{
		{0.5f, 0.5f, 0.f},
		{0,0},
		0.f,
		color,
		Transform
	};

	Vertex v4
	{
		{0.5f, -0.5f, 0.f},
		{0,0},
		0.f,
		color,
		Transform
	};

	m_Verticies.push_back(v1);
	m_Verticies.push_back(v2);
	m_Verticies.push_back(v3);
	m_Verticies.push_back(v4);
}
