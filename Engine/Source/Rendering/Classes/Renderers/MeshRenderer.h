#pragma once
#include "Rendering/RenderCore.h"
#include "Rendering/Renderer.h"
#include "Utils/Task.h"

struct DENGINE_API DirectionalLight
{
	color4 Direction;
	color4 Radiance;
};

//parameters that can differ for each mesh within 1 draw call
struct DENGINE_API MeshData
{
	glm::mat4 Matrix;
};

struct MeshDrawCall
{
	//different data (size of this determines instance count)
	std::vector<MeshData> MeshDataArray;

	//same material for each mesh
	Ref<Material> CommonMaterial;

	//same mesh for each draw call
	Ref<Mesh> CommonMesh;
};

/*
	renderer for both dynamic and static meshes, 
	static meshes with the same materials and meshes get batched into 1 draw call
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

	void SubmitStaticMesh(Ref<StaticMesh> mesh);
	void RemoveStaticMesh(Ref<StaticMesh> mesh);

	void GenDrawCalls();

private:

	std::vector<MeshDrawCall> m_DrawCalls;

	//dynamic mesh array (for submition)
	std::vector<Ref<Mesh>> m_MeshBuffer;

	//static mesh array (for submition)
	std::vector<Ref<StaticMesh>> m_StaticMeshBuffer;

	//same layout for any mesh
	Ref<VertexBufferLayout> m_MeshVertexBufferLayout;

	//shared between draw calls
	Ref<ShaderStorageBuffer> m_MeshDataBuffer;

	//lights
	Ref<ShaderStorageBuffer> m_LightDataBuffer;
	std::vector<DirectionalLight> m_DirectionalLights;
};

