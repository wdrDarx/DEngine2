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

struct CUSTOMCLASSES GLQuad
{
public:
	GLQuad() {};
	GLQuad(const glm::vec4& vi1, const glm::vec4& vi2, const glm::vec4& vi3, const glm::vec4& vi4, const float& slot, const glm::vec2& uv, const glm::vec4& color)
	{
		v1 = vi1;
		v2 = vi2;
		v3 = vi3;
		v4 = vi4;

		textureSlot = slot;
		UVScale = uv;
		Color = color;
	}
	glm::vec4 v1;
	glm::vec4 v2;
	glm::vec4 v3;
	glm::vec4 v4;

	float textureSlot;
	glm::vec2 UVScale;
	glm::vec4 Color;
};


class CUSTOMCLASSES QuadRenderer :  public Renderer
{
public:
	OBJECT_CLASS_DEF(QuadRenderer, Renderer)

	void OnConstruct() override;

	void BeginFrame() override;
	void EndFrame() override;

	void DrawQuad2D(const vec2d& pos, const vec2d& scale, const color4& color);
public:
	Ref<Shader> m_QuadShader;

	const std::vector<uint> m_QuadIndecies = { 0, 1, 2, 2, 3, 0};

	std::vector<Vertex> m_Verticies;

	Ref<VertexBuffer> m_VertexBuffer;
	Ref<VertexBufferLayout> m_VertexBufferLayout;
	Ref<VertexArray> m_VertexArray;
	Ref<IndexBuffer> m_IndexBuffer;

	glm::mat4 m_ViewProjectionMatrix;
};

