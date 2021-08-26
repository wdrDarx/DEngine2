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
	};

	struct Instance
	{
		glm::mat4 Matrix;
		color4 color;
		color4 TextureIndex; //Actually just the index in X
	};

	struct QuadRendererDrawCall
	{
		QuadRendererDrawCall(Ref<Texture> blankTexture)
		{
			if (blankTexture)
				TextureBindings.push_back(blankTexture->GetBinding());
		}
		std::vector<uint> TextureBindings;
		std::vector<Instance> Instances;
	};

	struct Quad
	{
		Transform trans;
		color4 color;
		Ref<Texture> texture;
	};
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

	//shared constant data of quad positions and texture coords
	Ref<VertexBuffer> m_QuadCommonBuffer;
	Ref<VertexArray> m_QuadCommonArray;
	Ref<IndexBuffer> m_QuadCommonIndexBuffer;

	//per vertex layout
	Ref<VertexBufferLayout> m_VertexBufferLayout;

	//per isntance layout (remember to set a layout offset at the start)
	Ref<VertexBufferLayout> m_InstanceBufferLayout;

	//consts per quad
	const std::vector<uint> m_QuadIndecies = { 0, 1, 2, 2, 3, 0 };
	const std::vector<vec2d> m_QuadTexCoords = { {0,0}, {0,1}, {1,1}, {1,0} };
	const std::vector<vec3d> m_QuadPositions = { vec3d{-0.5f, -0.5f, 0.f}, vec3d{-0.5f, 0.5f, 0.f}, vec3d{0.5f, 0.5f, 0.f}, vec3d{0.5f, -0.5f, 0.f} };
	
};

