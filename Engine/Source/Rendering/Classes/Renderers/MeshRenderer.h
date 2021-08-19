#pragma once
#include "Rendering/RenderCore.h"
#include "Rendering/Renderer.h"
#include "Utils/Task.h"

struct DENGINE_API MeshData
{
	glm::mat4 Matrix;
};

struct DENGINE_API SingleMeshBuffer
{
	Ref<Mesh> mesh;
	MeshData meshData;
};

/*
	Dynamic mesh renderer
*/
class DENGINE_API MeshRenderer : public Renderer
{
public:
	OBJECT_CLASS_DEF(MeshRenderer, Renderer)

	void OnConstruct() override;

	void PrepareFrame() override;
	void RenderFrame(Ref<Camera> camera) override;
	void ClearFrame() override;

	void SubmitMesh(Ref<Mesh> mesh);
	void RemoveMesh(Ref<Mesh> mesh);

private:
	std::vector<MeshData> m_MeshDataArray;
	std::vector<SingleMeshBuffer> m_MeshBuffer;
	Ref<VertexBufferLayout> m_MeshVertexBufferLayout;
	Ref<ShaderStorageBuffer> m_MeshDataBuffer;
};

