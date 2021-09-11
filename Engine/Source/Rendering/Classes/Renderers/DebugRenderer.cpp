#include "DebugRenderer.h"
#include "DEngine.h"

void DebugRenderer::OnConstruct()
{
	Super::OnConstruct();

	m_StorageBuffer = MakeRef<ShaderStorageBuffer>(0); //binding = 0
}

void DebugRenderer::PrepareFrame()
{
	Super::PrepareFrame();

	if (!GetPipeline()->GetRenderAPI()->IsShaderInCache("LineShader"))
		GetPipeline()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\LineShader.shader"), "LineShader");
	if (!GetPipeline()->GetRenderAPI()->IsShaderInCache("WireframeShader"))
		GetPipeline()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\WireframeShader.shader"), "WireframeShader");
		
	//draw physx stuff
	if (GetPipeline()->GetScene()->GetApplication()->GetAppType() == AppType::DEVELOPMENT && GetRenderFlags() & RenderFlags::PHYSX)
	{ 
		if(GetPipeline()->GetScene()->GetPhysicsScene() && GetPipeline()->GetScene()->GetPhysicsScene()->GetPhysXScene())
		{
			const physx::PxRenderBuffer& rb = GetPipeline()->GetScene()->GetPhysicsScene()->GetPhysXScene()->getRenderBuffer();
			for (physx::PxU32 i = 0; i < rb.getNbLines(); i++)
			{
				const physx::PxDebugLine& line = rb.getLines()[i];
				vec4d start = vec4d(PhysicsUtils::FromPhysXVector(line.pos0), 1.0f);
				vec4d end = vec4d(PhysicsUtils::FromPhysXVector(line.pos1), 1.0f);
				m_LineBuffer.push_back({ start, end, color4{0,1,0,1} });
			}
		}
	}

	//Grid
	if (m_RenderFlags & RenderFlags::Enum::GRID)
	{
		DrawGrid(100.f, 1000.f);
	}
}

void DebugRenderer::RenderFrame(Ref<Camera> camera)
{
	Super::RenderFrame(camera);

	//draw in dev mode only
	if (GetPipeline()->GetScene()->GetApplication()->GetAppType() != AppType::DEVELOPMENT)
		return;

	if(m_LineBuffer.size() < 1 && m_CubeBuffer.size() < 1) return;

	//Draw Lines
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(m_LineWidth);

	if (GetRenderFlags() & RenderFlags::DEBUGLINES)
	{
		Ref<Shader> lineShader = GetPipeline()->GetRenderAPI()->GetShaderFromCache("LineShader");
		lineShader->Bind();
		lineShader->SetUniformMat4f("u_ViewProjectionMatrix", camera->GetViewProjectionMatrix());
		m_StorageBuffer->SetData(m_LineBuffer.data(), m_LineBuffer.size() * sizeof(LineSegment));
		glDrawArraysInstanced(GL_LINES, 0, 2, m_LineBuffer.size()); //draw m_LineBuffer.size() line segments (each have a start, end and color)
		GetPipeline()->GetRenderAPI()->GetRenderStats().DrawCalls++;
	}

	//Draw Cubes
	//Wireframe
	if(GetRenderFlags() & RenderFlags::DEBUGCUBES)
	{ 
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		Ref<Shader> WireframeShader = GetPipeline()->GetRenderAPI()->GetShaderFromCache("WireframeShader");
		WireframeShader->Bind();
		WireframeShader->SetUniformMat4f("u_ViewProjectionMatrix", camera->GetViewProjectionMatrix());
		for (auto& cube : m_CubeBuffer)
		{
			WireframeShader->SetUniformMat4f("u_Model", World::MakeMatrix(cube.trans));
			WireframeShader->SetUniform4f("u_Color", cube.color.r, cube.color.g, cube.color.b, 1.0f);
			RenderUtils::RenderCube();
			GetPipeline()->GetRenderAPI()->GetRenderStats().DrawCalls++;
		}
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}

void DebugRenderer::ClearFrame()
{
	Super::ClearFrame();

	m_LineBuffer.clear();
	m_CubeBuffer.clear();
}

void DebugRenderer::DrawDebugLine(const vec3d& start, const vec3d& end, const color3& color)
{
	color4 out;
	out.r = color.r;
	out.g = color.g;
	out.b = color.b;
	out.a = 1.0f;

	DrawDebugLine(start, end, out);
}

void DebugRenderer::DrawDebugLine(const vec3d& start, const vec3d& end, const color4& color)
{
	m_LineBuffer.push_back({ vec4d(start, 1.0) ,  vec4d(end, 1.0), color });
}

void DebugRenderer::DrawDebugLineStrip(const std::vector<vec3d>& positions, const color3& color)
{
	color4 out;
	out.r = color.r;
	out.g = color.g;
	out.b = color.b;
	out.a = 1.0f;

	DrawDebugLineStrip(positions, out);
}

void DebugRenderer::DrawDebugLineStrip(const std::vector<vec3d>& positions, const color4& color)
{
	for (uint i = 0; i < positions.size(); i++)
	{
		if (i > positions.size() - 2) break;

		m_LineBuffer.push_back({ vec4d(positions[i], 1.0) ,  vec4d(positions[i + 1], 1.0), color });
	}
}

void DebugRenderer::DrawDebugCube(const vec3d& pos, const vec3d& rot, const vec3d& size, const color3& color)
{
	m_CubeBuffer.push_back({{pos, rot, size}, color});
}

void DebugRenderer::DrawGrid(float DivisionSize, float TotalSize, const color4& color)
{
	int divs = TotalSize / DivisionSize;
	for (int x = -divs; x <= divs; x++)
	{
		float Offset = x * DivisionSize;
		DrawDebugLine({Offset, 0, -TotalSize}, { Offset, 0, TotalSize}, color);
	}

	for (int x = -divs; x <= divs; x++)
	{
		float Offset = x * DivisionSize;
		DrawDebugLine({ -TotalSize, 0, Offset }, {TotalSize, 0, Offset}, color);
	}
}
