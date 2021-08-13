#pragma once
#include "Rendering/RenderCore.h"
#include "Rendering/Renderer.h"
#include "Utils/Task.h"

namespace QR
{ 
	static const uint MaxQuads = 100000;
	static const uint MaxVerts = MaxQuads * 4;
	static const uint MaxTextures = 32;
	static const uint MaxDrawCalls = 100;

	struct Vertex
	{
		vec3d position;
		vec2d TexCoord;
		float TextureSlot;
		color4 color;
		float Matrixindex;
	};

	struct QuadStorageBuffer
	{
		color4 Color;
	};

	struct QuadRendererDrawCall
	{
		QuadRendererDrawCall(Ref<Texture> blankTexture = nullptr)
		{
			vertexArray = MakeRef<VertexArray>();
			indexBuffer = MakeRef<IndexBuffer>();
			vertexBuffer = MakeRef<VertexBuffer>();

			if (blankTexture)
				TextureBindings.push_back(blankTexture->m_RendererID);
		}

		Ref<VertexBuffer> vertexBuffer;
		Ref<VertexArray> vertexArray;
		Ref<IndexBuffer> indexBuffer;
		std::vector<uint> TextureBindings;
		std::vector<glm::mat4> Matricies;

		std::vector<Vertex> Verticies;
	};

	struct Quad
	{
		Transform trans;
		color4 color;
		Ref<Texture> texture;
	};
}



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
	void DrawQuad(const Transform& trans, const color4& color, Ref<Texture> texture = nullptr);

	void ProcessQuads();

public:
	JobPool m_JobPool;
	Ref<Texture> m_BlankTexture;
	std::vector<QR::Quad> m_QuadBuffer;

	std::vector<QR::QuadRendererDrawCall> m_DrawCalls;
	int m_CurrentDrawCallIndex = -1;

	//storage buffer is shared
	Ref<ShaderStorageBuffer> m_StorageBuffer;

	const std::vector<uint> m_QuadIndecies = { 0, 1, 2, 2, 3, 0 };
	Ref<VertexBufferLayout> m_VertexBufferLayout;
};

