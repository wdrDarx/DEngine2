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

void QuadRenderer::RenderFrame(Ref<Camera> camera)
{
	Super::RenderFrame(camera);

	glEnable(GL_DEPTH_TEST);

	if (m_Verticies.size() == 0) return;

	m_QuadShader->Bind();

	for (uint i = 0; i < m_CurrentTextureBinding; i++)
	{			
 		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_TextureBindings[i]);
		m_QuadShader->SetUniform1i("u_Textures[" + STRING(i) + "]", i);
	}

	m_QuadShader->SetUniformMat4f("u_ViewProjectionMatrix", camera->GetViewProjectionMatrix());
	GetScene()->GetRenderAPI()->DrawIndexed(*m_QuadShader, *m_VertexArray, *m_IndexBuffer);

	glDisable(GL_DEPTH_TEST);
}

void QuadRenderer::PrepareFrame()
{
	Super::PrepareFrame();

	if(m_Verticies.size() == 0) return;
	vec2d size = GetScene()->GetRenderAPI()->GetViewportSize();
	m_VertexBuffer->SetNewData(m_Verticies.data(), sizeof(Vertex) * m_Verticies.size());
	std::vector<uint> indexbuffer;
	uint quadCount = m_Verticies.size() / 4;
	for(uint i = 0; i < quadCount; i++)
	{ 		
		//indexbuffer.insert(indexbuffer.end(), m_QuadIndecies.begin(), m_QuadIndecies.end());
		for (uint j = 0; j < m_QuadIndecies.size(); j++)
		{
			uint ogIndex = m_QuadIndecies[j];

			indexbuffer.push_back(ogIndex + i * 4);
		}
	}
	m_IndexBuffer->SetData(indexbuffer.data(), indexbuffer.size());
}

void QuadRenderer::ClearFrame()
{
	Super::ClearFrame();

	m_Verticies.clear();
	
	for (uint i = 0; i < 32; i++)
	{
		m_TextureBindings[i] = 0;
	}

	m_CurrentTextureBinding = 0;
}

void QuadRenderer::DrawQuad2D(const vec2d& pos, const vec2d& scale, const color4& color)
{
	glm::mat4 Transform = glm::translate(glm::mat4(1.0f), {pos.x, pos.y, 0.f}) * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.f});

	DrawQuad(Transform, color);

}

void QuadRenderer::DrawQuad3D(const vec3d& size, const Transform& trans, const color4& color, Ref<Texture> texture)
{
	Transform final = trans;
	final.scale *= size;

	glm::mat4 Transform = World::MakeMatrix(final);

	DrawQuad(Transform, color, texture);
}

void QuadRenderer::DrawQuad(const glm::mat4& matrix, const color4& color, Ref<Texture> texture)
{
	float TextureID = 0.f;
	if(texture)
	{
		m_TextureBindings[m_CurrentTextureBinding] = texture->m_RendererID;
		TextureID = m_CurrentTextureBinding;
		m_CurrentTextureBinding++;
	}

	Vertex v1
	{
		{-0.5f, -0.5f, 0.f},
		{0,0},
		TextureID,
		color,
		matrix
	};

	Vertex v2
	{
		{-0.5f, 0.5f, 0.f},
		{0,1},
		TextureID,
		color,
		matrix
	};

	Vertex v3
	{
		{0.5f, 0.5f, 0.f},
		{1,1},
		TextureID,
		color,
		matrix
	};

	Vertex v4
	{
		{0.5f, -0.5f, 0.f},
		{1,0},
		TextureID,
		color,
		matrix
	};

	m_Verticies.push_back(v1);
	m_Verticies.push_back(v2);
	m_Verticies.push_back(v3);
	m_Verticies.push_back(v4);
}
