#include "QuadRenderer.h"
#include "DEngine.h"

void QuadRenderer::OnConstruct()
{
	Super::OnConstruct();

	m_BlankTexture = MakeRef<Texture>();

	m_DrawCalls.reserve(MaxDrawCalls);
	for (uint i = 0; i < MaxDrawCalls; i++)
	{
		m_DrawCalls.push_back(QuadRendererDrawCall(m_BlankTexture));
	}
	m_CurrentDrawCallIndex = -100;
	m_VertexBufferLayout = MakeRef<VertexBufferLayout>();

	m_VertexBufferLayout->Push<float>(3); //position
	m_VertexBufferLayout->Push<float>(2); //texture coords
	m_VertexBufferLayout->Push<float>(1); //texture index
	m_VertexBufferLayout->Push<float>(4); //color

	m_VertexBufferLayout->Push<float>(4); //matrix
	m_VertexBufferLayout->Push<float>(4);
	m_VertexBufferLayout->Push<float>(4);
	m_VertexBufferLayout->Push<float>(4);
}

void QuadRenderer::RenderFrame(Ref<Camera> camera)
{
	Super::RenderFrame(camera);

	if(m_CurrentDrawCallIndex < 0) return;

	glEnable(GL_DEPTH_TEST);
	GetScene()->GetRenderAPI()->GetShaderFromCache("QuadShader")->Bind();
	GetScene()->GetRenderAPI()->GetShaderFromCache("QuadShader")->SetUniformMat4f("u_ViewProjectionMatrix", camera->GetViewProjectionMatrix());

	//draw each draw call
	for (int i = 0; i <= m_CurrentDrawCallIndex; i++)
	{
		auto& call = m_DrawCalls[i];

		for (uint i = 0; i < call.TextureBindings.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, call.TextureBindings[i]);
			GetScene()->GetRenderAPI()->GetShaderFromCache("QuadShader")->SetUniform1i("u_Textures[" + STRING(i) + "]", i);
		}

		GetScene()->GetRenderAPI()->DrawIndexed(*GetScene()->GetRenderAPI()->GetShaderFromCache("QuadShader"), *call.vertexArray, *call.indexBuffer);
	}
	glDisable(GL_DEPTH_TEST);
}

void QuadRenderer::PrepareFrame()
{
	Super::PrepareFrame();

	if(!GetScene()->GetRenderAPI()->IsShaderInCache("QuadShader"))
		GetScene()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\QuadShader.shader"), "QuadShader");

	if(m_CurrentDrawCallIndex < 0) return;

	for(int i = 0; i <= m_CurrentDrawCallIndex; i++)
	{
		auto& call = m_DrawCalls[i];

		//copy over vertex data
		call.vertexBuffer->SetNewData(call.Verticies.data(), (uint)sizeof(Vertex) * (uint)call.Verticies.size());

		//generate index buffer
		std::vector<uint> indexbuffer;
		uint quadCount = (uint)call.Verticies.size() / 4;
		for (uint i = 0; i < quadCount; i++)
		{
			for (uint j = 0; j < m_QuadIndecies.size(); j++)
			{
				uint ogIndex = m_QuadIndecies[j];

				indexbuffer.push_back(ogIndex + i * 4);
			}
		}

		//set index buffer
		call.indexBuffer->SetData(indexbuffer.data(), (uint)indexbuffer.size());

		//set vertex array
		call.vertexArray->Addbuffer(*call.vertexBuffer, *m_VertexBufferLayout);
	}
}

void QuadRenderer::ClearFrame()
{
	Super::ClearFrame();

	for(int i = 0; i <= m_CurrentDrawCallIndex; i++)	
	{ 
		m_DrawCalls[i].Verticies.clear();
	}

	m_CurrentDrawCallIndex = -100;
}

void QuadRenderer::DrawQuad2D(const vec2d& pos, const vec2d& scale, const color4& color)
{
	glm::mat4 Transform = glm::translate(glm::mat4(1.0f), { pos.x, pos.y, 0.f }) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.f });

	DrawQuad(Transform, color);
}

void QuadRenderer::DrawQuad3D(const vec2d& size, const Transform& trans, const color4& color, Ref<Texture> texture)
{
	Transform final = trans;
	final.scale.x *= size.x;
	final.scale.y *= size.y;

	glm::mat4 Transform = World::MakeMatrix(final);

	DrawQuad(Transform, color, texture);
}

void QuadRenderer::DrawQuad(const glm::mat4& matrix, const color4& color, Ref<Texture> texture)
{
	if(!GetScene()->GetRenderAPI()) return;

	if(m_CurrentDrawCallIndex < 0)
		m_CurrentDrawCallIndex = 0;

	QuadRendererDrawCall* CurrentDrawCall = &m_DrawCalls[m_CurrentDrawCallIndex];

	//do we need to create a new draw call?
	if (CurrentDrawCall->Verticies.size() >= MaxVerts || CurrentDrawCall->TextureBindings.size() >= MaxTextures)
	{
		m_CurrentDrawCallIndex++;
		CurrentDrawCall = &m_DrawCalls[m_CurrentDrawCallIndex];
	}

	float TextureID = 0.f;
	if (texture)
	{
		bool found = false;
		for (int i = 0; i < CurrentDrawCall->TextureBindings.size(); i++)
		{
			if(texture->m_RendererID == CurrentDrawCall->TextureBindings[i])
			{
				TextureID = (float)i;
				found = true;
				break;
			}
		}

		if(!found)
		{ 
			CurrentDrawCall->TextureBindings.push_back(texture->m_RendererID);
			TextureID = (float)CurrentDrawCall->TextureBindings.size() - 1.f;
		}
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

	CurrentDrawCall->Verticies.push_back(v1);
	CurrentDrawCall->Verticies.push_back(v2);
	CurrentDrawCall->Verticies.push_back(v3);
	CurrentDrawCall->Verticies.push_back(v4);
}
