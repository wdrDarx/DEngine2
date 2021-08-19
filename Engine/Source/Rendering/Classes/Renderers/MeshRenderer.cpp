#include "MeshRenderer.h"
#include "DEngine.h"

void MeshRenderer::OnConstruct()
{
	m_MeshDataBuffer = MakeRef<ShaderStorageBuffer>(0); //binding = 0

	m_MeshVertexBufferLayout = MakeRef<VertexBufferLayout>();
	m_MeshVertexBufferLayout->Push<float>(3); //vertex pos
	m_MeshVertexBufferLayout->Push<float>(3); //Vertex normal
	m_MeshVertexBufferLayout->Push<float>(2); //texture coords
}

void MeshRenderer::PrepareFrame()
{
	bool regenStorage = false;
	for (auto& mesh : m_MeshBuffer)
	{
		m_MeshDataArray.push_back({ mesh.mesh->GetMatrix() });
	}
}

void MeshRenderer::RenderFrame(Ref<Camera> camera)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_MeshDataBuffer->Bind();
	m_MeshDataBuffer->SetData(m_MeshDataArray.data(), m_MeshDataArray.size() * sizeof(MeshData));
	for (uint i = 0; i < m_MeshBuffer.size(); i++)
	{	
		auto& mesh = m_MeshBuffer[i];
		if(!mesh.mesh || !mesh.mesh->GetMaterial()) continue;

		mesh.mesh->Bind(GetScene()->GetRenderAPI());
		mesh.mesh->GetMaterial()->GetShader()->SetUniform1f("u_MeshDataIndex", (float)i);
		
		GetScene()->GetRenderAPI()->DrawIndexed(*mesh.mesh->GetIndexBuffer());
	}
	m_MeshDataBuffer->Unbind();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}

void MeshRenderer::ClearFrame()
{
	m_MeshDataArray.clear();
}

void MeshRenderer::SubmitMesh(Ref<Mesh> mesh)
{
	m_MeshBuffer.push_back({mesh, MeshData()});
	mesh->AssignVertexBufferLayout(m_MeshVertexBufferLayout);
}

void MeshRenderer::RemoveMesh(Ref<Mesh> mesh)
{
	auto remove = m_MeshBuffer.end();
	for (auto it = m_MeshBuffer.begin(); it != m_MeshBuffer.end(); it++)
	{
		if (mesh == (*it).mesh)
		{
			remove = it;
			break;
		}
	}

	if (remove != m_MeshBuffer.end())
	{
		m_MeshBuffer.erase(remove);
	}
}
