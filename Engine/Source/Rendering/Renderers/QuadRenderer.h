#pragma once
#include "Rendering/RenderCore.h"
#include "Rendering/Renderers/Renderer.h"
#include "Utils/Task.h"

static const uint MaxQuads = 100000;
static const uint MaxVerts = MaxQuads * 4;
static const uint MaxTextures = 32;
static const uint MaxDrawCalls = 2;

struct DENGINE_API Vertex
{
	glm::vec3 position;
	glm::vec2 TexCoord;
	float TextureSlot;
	glm::vec4 color;
	glm::mat4 trans;
};

struct DENGINE_API QuadRendererDrawCall
{
	QuadRendererDrawCall(Ref<Texture> blankTexture = nullptr)
	{
		vertexArray = MakeRef<VertexArray>();
		indexBuffer = MakeRef<IndexBuffer>();
		vertexBuffer = MakeRef<VertexBuffer>();

		if(blankTexture)
			TextureBindings.push_back(blankTexture->m_RendererID);
	}

	Ref<VertexBuffer> vertexBuffer;
	Ref<VertexArray> vertexArray;
	Ref<IndexBuffer> indexBuffer;
	std::vector<uint> TextureBindings;

	std::vector<Vertex> Verticies;
};

struct DENGINE_API Quad2D
{
	vec2d pos;
	vec2d scale;
	color4 color;
};

class DENGINE_API QuadRenderer : public Renderer
{
public:
	OBJECT_CLASS_DEF(QuadRenderer, Renderer)

	void OnConstruct() override;

	void PrepareFrame() override;
	void RenderFrame(Ref<Camera> camera) override;
	void ClearFrame() override;

	void DrawQuad2D(const vec2d& pos, const vec2d& scale, const color4& color);
	void DrawQuad3D(const vec2d& size, const Transform& trans, const color4& color, Ref<Texture> texture = nullptr);
	void DrawQuad(const glm::mat4& matrix, const color4& color, Ref<Texture> texture = nullptr);

public:
	Ref<Texture> m_BlankTexture;

	std::vector<QuadRendererDrawCall> m_DrawCalls;
	int m_CurrentDrawCallIndex = -1;

	const std::vector<uint> m_QuadIndecies = { 0, 1, 2, 2, 3, 0 };
	Ref<VertexBufferLayout> m_VertexBufferLayout;
};

