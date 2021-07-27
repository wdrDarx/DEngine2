#pragma once
#include "DEngine.h"
#include "CustomClasses.h"

struct CUSTOMCLASSES Vertex
{
	glm::vec3 position;
	glm::vec2 TexCoord;
	float TextureSlot;
	glm::vec4 color;
	glm::mat4 trans;
};

struct CUSTOMCLASSES Quad2D
{
	vec2d pos;
	vec2d scale;
	color4 color;
};

class CUSTOMCLASSES QuadRenderer :  public Renderer
{
public:
	OBJECT_CLASS_DEF(QuadRenderer, Renderer)

	void OnConstruct() override;

	void PrepareFrame() override;
	void RenderFrame(Ref<Camera> camera) override;
	void ClearFrame() override;

	void DrawQuad2D(const vec2d& pos, const vec2d& scale, const color4& color);
	void DrawQuad3D(const vec3d& size, const Transform& trans, const color4& color, float TextureID = 0.f);
	void DrawQuad(const glm::mat4& matrix, const color4& color, float TextureID = 0.f);
public:
	Ref<Shader> m_QuadShader;

	const std::vector<uint> m_QuadIndecies = { 0, 1, 2, 2, 3, 0};

	std::vector<Vertex> m_Verticies;

	Ref<VertexBuffer> m_VertexBuffer;
	Ref<VertexBufferLayout> m_VertexBufferLayout;
	Ref<VertexArray> m_VertexArray;
	Ref<IndexBuffer> m_IndexBuffer;

	glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);
};

