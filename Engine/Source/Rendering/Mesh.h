#pragma once
#include "Core/Core.h"
#include "RenderAPI.h"
#include "Vertexbuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Indexbuffer.h"
#include "Material.h"

struct DENGINE_API MeshVertex
{
	vec3d Pos;
	vec3d Norm;
	vec3d Tangent;
	vec3d Bitangent;
	vec2d TexCoord;

	bool operator==(const MeshVertex& other) const
	{
		return Pos == other.Pos && 
		Norm == other.Norm  &&
		Tangent == other.Tangent && 
		Bitangent == other.Bitangent &&
		TexCoord == other.TexCoord;
	}
};

class DENGINE_API Mesh
{
public:
	Mesh();

	void SetData(const std::vector<MeshVertex>& verts, const std::vector<uint>& indicies);
	void AssignVertexBufferLayout(Ref<VertexBufferLayout> layout);
	void Bind(Ref<RenderAPI> renderApi);
	void RegenerateBuffers();

	void SetTransform(const Transform& trans);
	const glm::mat4& GetMatrix() const;
	const Transform& GetTransform() const;


	const std::vector<MeshVertex>& GetVerticies() const
	{
		return m_Verticies;
	}

	const std::vector<uint>& GetIndicies() const
	{
		return m_Indicies;
	}

	Ref<VertexBuffer> GetVertexBuffer()
	{
		return m_VertexBuffer;
	}

	Ref<VertexArray> GetVertexArray()
	{
		return m_VertexArray;
	}

	Ref<IndexBuffer> GetIndexBuffer()
	{
		return m_IndexBuffer;
	}

	bool isFlaggedChange() const
	{
		return m_FlagChanged;
	}

	void FlagChanged()
	{
		m_FlagChanged = true;
	}

	void UnflagChanged()
	{
		m_FlagChanged = false;
	}

	Ref<Material> GetMaterial()
	{
		return m_Material;
	}

	void SetMaterial(Ref<Material> material)
	{
		m_Material = material;
	}

	bool IsValidForDraw()
	{
		return GetMaterial() && m_VertexArray->m_VertexBuffer;
	}

private:
	void UpdateMatrix();

	Ref<Material> m_Material;

	std::vector<MeshVertex> m_Verticies;
	std::vector<uint> m_Indicies;

	Ref<VertexBuffer> m_VertexBuffer;
	Ref<VertexArray> m_VertexArray;
	Ref<IndexBuffer> m_IndexBuffer;
	
	glm::mat4 m_Matrix;
	Transform m_Transform;

	bool m_FlagChanged;
};