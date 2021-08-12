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

	//Timer PrepareFrametimer;
	ProcessQuads();
	//LogTemp("Seconds to prepare quads : " + STRING(PrepareFrametimer.GetSecondsElapsed()));

	if(m_CurrentDrawCallIndex < 0) return;

	for(int i = 0; i <= m_CurrentDrawCallIndex; i++)
	{
		QuadRendererDrawCall& call = m_DrawCalls[i];

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

#if 0
	uint threads = m_JobPool.numThreads;
	threads = 20;
	uint QuadsPerThread = ceil((float)m_QuadBuffer.size() / (float)threads);

	std::mutex mu;
	uint T_CurrentThreadIndex = 0;
	for(uint i = 0; i < threads; i++)
	{
	m_JobPool.Execute([&]()
	{
		mu.lock();
		uint threadIndex = T_CurrentThreadIndex;
		T_CurrentThreadIndex++;
		mu.unlock();
	
		bool quit = false;
		uint StartQuadIndex = threadIndex * QuadsPerThread;
		if (threadIndex > 0) StartQuadIndex++; //otherwise it repeats the last EndQuadIndex

		uint EndQuadIndex = MIN(StartQuadIndex + QuadsPerThread, m_QuadBuffer.size() - 1);

		if (EndQuadIndex >= m_QuadBuffer.size() - 1) //should quit after the thread
			quit = true;

		for (uint Quadindex = StartQuadIndex; Quadindex <= EndQuadIndex; Quadindex++)
		{			
			Quad& currentQuad = m_QuadBuffer[Quadindex];

			mu.lock();
			//do we need to create a new draw call?
			QuadRendererDrawCall* CurrentDrawCall = &m_DrawCalls[m_CurrentDrawCallIndex];
			if (CurrentDrawCall->Verticies.size() >= MaxVerts || CurrentDrawCall->TextureBindings.size() >= MaxTextures)
			{
				m_CurrentDrawCallIndex++;
				CurrentDrawCall = &m_DrawCalls[m_CurrentDrawCallIndex];
			}
			else
			{
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
			mu.unlock();

			glm::mat4 matrix = World::MakeMatrix(currentQuad.trans);
			const color4& color = currentQuad.color;

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

			mu.lock();
			CurrentDrawCall->Verticies.push_back(v1);
			CurrentDrawCall->Verticies.push_back(v2);
			CurrentDrawCall->Verticies.push_back(v3);
			CurrentDrawCall->Verticies.push_back(v4);
			mu.unlock();
		}	

		if (quit)
			return;
	});
	}

	m_JobPool.Wait();
#else
	std::vector<glm::mat4> T_Matricies;
	T_Matricies.resize(m_QuadBuffer.size());

	uint Threads = m_JobPool.numThreads;
	Threads = 10;
	uint T_CurrentThread = 0;
	uint QuadsPerThread = ceil((float)m_QuadBuffer.size() / (float)Threads);

	std::mutex mu;
	for (uint i = 0; i < Threads; i++)
	{
		m_JobPool.Execute([&]()
		{
			mu.lock();
			uint MyThreadIndex = T_CurrentThread;
			T_CurrentThread++;
			mu.unlock();

			uint StartIndex = MyThreadIndex * QuadsPerThread;
			uint EndIndex = MIN(StartIndex + QuadsPerThread, m_QuadBuffer.size() - 1);
			if (StartIndex >= m_QuadBuffer.size()) return;

			uint matrixCount = (EndIndex - StartIndex) + 1;

			std::vector<glm::mat4> MyMatricies;
			MyMatricies.resize(matrixCount);

			for (uint index = StartIndex; index <= EndIndex; index++)
			{
				MyMatricies[index - StartIndex] = World::MakeMatrix(m_QuadBuffer[index].trans);
			}

			mu.lock();
			uptr target = (uptr)T_Matricies.data() + (StartIndex * sizeof(glm::mat4));
			memcpy((void*)target, MyMatricies.data(), MyMatricies.size() * sizeof(glm::mat4));
			mu.unlock();
		});
	}

	m_JobPool.Wait();
	
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

		const glm::mat4& matrix = T_Matricies[i];
		const color4& color = currentQuad.color;

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
#endif
}

void QuadRenderer::DrawQuad(const Transform& trans, const color4& color, Ref<Texture> texture)
{
	m_QuadBuffer.push_back({trans, color, texture});
}

