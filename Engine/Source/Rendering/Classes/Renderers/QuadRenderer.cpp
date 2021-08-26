#include "QuadRenderer.h"
#include "DEngine.h"

using namespace QR;

void QuadRenderer::OnConstruct()
{
	Super::OnConstruct();

	//common vertex buffer (each quad uses this)
	m_QuadCommonBuffer = MakeRef<VertexBuffer>();
	std::vector<Vertex> ConstVerts;
	ConstVerts.push_back({m_QuadPositions[0], m_QuadTexCoords[0]});
	ConstVerts.push_back({m_QuadPositions[1], m_QuadTexCoords[1]});
	ConstVerts.push_back({m_QuadPositions[2], m_QuadTexCoords[2]});
	ConstVerts.push_back({m_QuadPositions[3], m_QuadTexCoords[3]});
	m_QuadCommonBuffer->SetData(ConstVerts.data(), ConstVerts.size() * sizeof(Vertex));

	//common index buffer per quad
	m_QuadCommonIndexBuffer = MakeRef<IndexBuffer>();
	m_QuadCommonIndexBuffer->SetData(m_QuadIndecies.data(), m_QuadIndecies.size());

	//per vertex layout 
	m_VertexBufferLayout = MakeRef<VertexBufferLayout>();
	m_VertexBufferLayout->Push<float>(3); //position
	m_VertexBufferLayout->Push<float>(2); //texture coords

	//common vertex array (contains common vertex buffer and common vertex buffer layout)
	m_QuadCommonArray = MakeRef<VertexArray>();
	m_QuadCommonArray->Addbuffer(*m_QuadCommonBuffer, *m_VertexBufferLayout);

	//matrix storage buffer per draw call
	m_StorageBuffer = MakeRef<ShaderStorageBuffer>(0); //layout = 0

	//blank texture (White 1x1)
	m_BlankTexture = MakeRef<Texture>(TextureSpec());

	//init threads
	m_JobPool.Initialize();

	//gen a const amount of draw calls
	m_DrawCalls.reserve(MaxDrawCalls);
	for (uint i = 0; i < MaxDrawCalls; i++)
	{
		m_DrawCalls.push_back(QuadRendererDrawCall(m_BlankTexture));
	}
	m_CurrentDrawCallIndex = -1;
}

void QuadRenderer::RenderFrame(Ref<Camera> camera)
{
	Super::RenderFrame(camera);

	if(m_CurrentDrawCallIndex < 0) return;

	glEnable(GL_DEPTH_TEST);
	GetPipeline()->GetRenderAPI()->GetShaderFromCache("QuadShader")->Bind();
	GetPipeline()->GetRenderAPI()->GetShaderFromCache("QuadShader")->SetUniformMat4f("u_ViewProjectionMatrix", camera->GetViewProjectionMatrix());

	m_StorageBuffer->Bind();
	//draw each draw call
	for (int i = 0; i <= m_CurrentDrawCallIndex; i++)
	{
		auto& call = m_DrawCalls[i];

		for (uint i = 0; i < call.TextureBindings.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, call.TextureBindings[i]);
			GetPipeline()->GetRenderAPI()->GetShaderFromCache("QuadShader")->SetUniform1i("u_Textures[" + STRING(i) + "]", i);
		}

		//set the storage buffer data
		m_StorageBuffer->SetData(call.Instances.data(), sizeof(Instance) * call.Instances.size());
		//draw instanced (call.Instances.size() is the instance count)
		GetPipeline()->GetRenderAPI()->DrawInstanced(*GetPipeline()->GetRenderAPI()->GetShaderFromCache("QuadShader"), *m_QuadCommonArray, *m_QuadCommonIndexBuffer, call.Instances.size());
	}

	m_StorageBuffer->Unbind();
	glDisable(GL_DEPTH_TEST);
}

void QuadRenderer::PrepareFrame()
{
	Super::PrepareFrame();

	if(!GetPipeline()->GetRenderAPI()->IsShaderInCache("QuadShader"))
		GetPipeline()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\QuadShader_Instanced.shader"), "QuadShader");

	//Timer PrepareFrametimer;
	ProcessQuads();
	//LogTemp("Seconds to prepare quads : " + STRING(PrepareFrametimer.GetSecondsElapsed()));
}

void QuadRenderer::ClearFrame()
{
	Super::ClearFrame();

	for(int i = 0; i <= m_CurrentDrawCallIndex; i++)	
	{ 
		m_DrawCalls[i].Instances.clear();
	}

	m_CurrentDrawCallIndex = -1;
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
	if (!GetPipeline()->GetRenderAPI()) return;
	if(m_QuadBuffer.size() < 1) return;

	if (m_CurrentDrawCallIndex < 0)
		m_CurrentDrawCallIndex = 0;
	
	for (uint i = 0; i < m_QuadBuffer.size(); i++)
	{
		Quad& currentQuad = m_QuadBuffer[i];
		QuadRendererDrawCall* CurrentDrawCall = &m_DrawCalls[m_CurrentDrawCallIndex];

		//do we need to create a new draw call?
		if (CurrentDrawCall->Instances.size() >= MaxQuads || CurrentDrawCall->TextureBindings.size() >= MaxTextures)
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
				if (currentQuad.texture->GetBinding() == CurrentDrawCall->TextureBindings[i])
				{
					TextureID = (float)i;
					found = true;
					break;
				}
			}

			if (!found)
			{
				CurrentDrawCall->TextureBindings.push_back(currentQuad.texture->GetBinding());
				TextureID = (float)CurrentDrawCall->TextureBindings.size() - 1.f;
			}
		}
		const color4& color = currentQuad.color;

		//push back the instance data
		CurrentDrawCall->Instances.push_back({ World::MakeMatrix(currentQuad.trans), color, color4(TextureID)});
	}
}

void QuadRenderer::DrawQuad(const Transform& trans, const color4& color, Ref<Texture> texture)
{
	m_QuadBuffer.push_back({trans, color, texture});
}

