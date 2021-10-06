#include "Mesh.h"
#include "DEngine.h"

Mesh::Mesh()
{
	m_VertexBuffer = MakeRef<VertexBuffer>();
	m_IndexBuffer = MakeRef<IndexBuffer>();
	m_VertexArray = MakeRef<VertexArray>();

	FlagChanged();
}

void Mesh::SetData(const std::vector<MeshVertex>& verts, const std::vector<uint>& indicies)
{
	m_Verticies = verts;
	m_Indicies = indicies;
	RegenerateBuffers();
}

void Mesh::AssignVertexBufferLayout(Ref<VertexBufferLayout> layout)
{
	m_VertexArray->Addbuffer(*m_VertexBuffer, *layout);
}

void Mesh::Bind(Ref<RenderAPI> renderApi)
{
	ASSERT(GetMaterial());
	GetMaterial()->Bind(renderApi);
	m_VertexArray->Bind();
}

void Mesh::RegenerateBuffers()
{
	m_VertexBuffer->SetData(m_Verticies.data(), m_Verticies.size() * sizeof(MeshVertex));
	m_IndexBuffer->SetData(m_Indicies.data(), m_Indicies.size());
	UpdateMatrix();
}

void Mesh::SetTransform(const Transform& trans)
{
	if(!(trans == m_Transform))
	{
		m_Transform = trans;
		UpdateMatrix();
	}	
}

const glm::mat4& Mesh::GetMatrix() const
{
	return m_Matrix;
}

const Transform& Mesh::GetTransform() const
{
	return m_Transform;
}

void Mesh::UpdateMatrix()
{
	m_Matrix = World::MakeMatrix(m_Transform);
	OnMatrixUpdate();
	FlagChanged();
}
