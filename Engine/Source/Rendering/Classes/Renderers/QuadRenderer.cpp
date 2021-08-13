#include "QuadRenderer.h"
#include "DEngine.h"

using namespace QR;

void QuadRenderer::OnConstruct()
{
	Super::OnConstruct();

	m_BlankTexture = MakeRef<Texture>();
	m_JobPool.Initialize();

	m_DrawCalls.reserve(MaxDrawCalls);
	for (uint i = 0; i < MaxDrawCalls; i++)
	{
		m_DrawCalls.push_back(QuadRendererDrawCall(m_BlankTexture));
	}
	m_CurrentDrawCallIndex = -100;
	m_VertexBufferLayout = MakeRef<VertexBufferLayout>();
	m_StorageBuffer = MakeRef<ShaderStorageBuffer>(2); //layout = 2

	m_VertexBufferLayout->Push<float>(3); //position
	m_VertexBufferLayout->Push<float>(2); //texture coords
	m_VertexBufferLayout->Push<float>(1); //texture index
	m_VertexBufferLayout->Push<float>(4); //color
	m_VertexBufferLayout->Push<float>(1); //matrix index
}

void QuadRenderer::RenderFrame(Ref<Camera> camera)
{
	Super::RenderFrame(camera);

	if(m_CurrentDrawCallIndex < 0) return;

	glEnable(GL_DEPTH_TEST);
	GetScene()->GetRenderAPI()->GetShaderFromCache("QuadShader")->Bind();
	GetScene()->GetRenderAPI()->GetShaderFromCache("QuadShader")->SetUniformMat4f("u_ViewProjectionMatrix", camera->GetViewProjectionMatrix());
	m_StorageBuffer->Bind();

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

		//set the matrix storage buffer
		m_StorageBuffer->SetData(call.Matricies.data(), sizeof(glm::mat4) * call.Matricies.size());
		GetScene()->GetRenderAPI()->DrawIndexed(*GetScene()->GetRenderAPI()->GetShaderFromCache("QuadShader"), *call.vertexArray, *call.indexBuffer);
	}
	glDisable(GL_DEPTH_TEST);
}

void QuadRenderer::PrepareFrame()
{
	Super::PrepareFrame();

	if(!GetScene()->GetRenderAPI()->IsShaderInCache("QuadShader"))
		GetScene()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\QuadShaderTest.shader"), "QuadShader");

	Timer PrepareFrametimer;
	ProcessQuads();
	LogTemp("Seconds to prepare quads : " + STRING(PrepareFrametimer.GetSecondsElapsed()));

	if(m_CurrentDrawCallIndex < 0) return;

	for(int i = 0; i <= m_CurrentDrawCallIndex; i++)
	{
		QuadRendererDrawCall& call = m_DrawCalls[i];

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

		//copy over vertex data
		call.vertexBuffer->SetData(call.Verticies.data(), (uint)sizeof(Vertex) * (uint)call.Verticies.size());

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
		m_DrawCalls[i].Matricies.clear();
	}

	m_CurrentDrawCallIndex = -100;
	m_QuadBuffer.clear();
}

void QuadRenderer::DrawQuad2D(const vec2d& pos, const vec2d& scale, const color4& color)
{
	Transform trans;
	trans.pos = {pos.x, pos.y, 0.0f};
	trans.scale = {scale.x, scale.y, 1.0f};

	DrawQuad(trans, color);
}

void QuadRenderer::DrawQuad3D(const vec2d& size, const Transform& trans, const color4& color, Ref<Texture> texture)
{
	Transform final = trans;
	final.scale.x *= size.x;
	final.scale.y *= size.y;
	DrawQuad(final, color, texture);
}

void QuadRenderer::ProcessQuads()
{
	if (!GetScene()->GetRenderAPI()) return;
	if(m_QuadBuffer.size() < 1) return;

	if (m_CurrentDrawCallIndex < 0)
		m_CurrentDrawCallIndex = 0;
	
	for (uint i = 0; i < m_QuadBuffer.size(); i++)
	{
		Quad& currentQuad = m_QuadBuffer[i];
		QuadRendererDrawCall* CurrentDrawCall = &m_DrawCalls[m_CurrentDrawCallIndex];

		//do we need to create a new draw call?
		if (CurrentDrawCall->Verticies.size() >= MaxVerts || CurrentDrawCall->TextureBindings.size() >= MaxTextures)
		{
			m_CurrentDrawCallIndex++;
			CurrentDrawCall = &m_DrawCalls[m_CurrentDrawCallIndex];
		}

		float TextureID = 0.f;
		if (currentQuad.texture)
		{
			bool found = false;
			for (int i = 0; i < CurrentDrawCall->TextureBindings.size(); i++)
			{
				if (currentQuad.texture->m_RendererID == CurrentDrawCall->TextureBindings[i])
				{
					TextureID = (float)i;
					found = true;
					break;
				}
			}

			if (!found)
			{
				CurrentDrawCall->TextureBindings.push_back(currentQuad.texture->m_RendererID);
				TextureID = (float)CurrentDrawCall->TextureBindings.size() - 1.f;
			}
		}

		CurrentDrawCall->Matricies.push_back(World::MakeMatrix(currentQuad.trans));
		float CurrentMatricIndex = (float)(CurrentDrawCall->Matricies.size() - 1);
		const color4& color = currentQuad.color;

		Vertex v1
		{
			{-0.5f, -0.5f, 0.f},
			{0,0},
			TextureID,
			color,
			CurrentMatricIndex
		};

		Vertex v2
		{
			{-0.5f, 0.5f, 0.f},
			{0,1},
			TextureID,
			color,
			CurrentMatricIndex
		};

		Vertex v3
		{
			{0.5f, 0.5f, 0.f},
			{1,1},
			TextureID,
			color,
			CurrentMatricIndex
		};

		Vertex v4
		{
			{0.5f, -0.5f, 0.f},
			{1,0},
			TextureID,
			color,
			CurrentMatricIndex
		};

		CurrentDrawCall->Verticies.push_back(v1);
		CurrentDrawCall->Verticies.push_back(v2);
		CurrentDrawCall->Verticies.push_back(v3);
		CurrentDrawCall->Verticies.push_back(v4);
	}
}

void QuadRenderer::DrawQuad(const Transform& trans, const color4& color, Ref<Texture> texture)
{
	m_QuadBuffer.push_back({trans, color, texture});
}

