#pragma once
#include "Rendering/RenderCore.h"
#include "Rendering/Renderer.h"
#include "Utils/Task.h"

struct DENGINE_API MeshRendererSettings
{
	float AmbientLightMultiplier = 0.5f;

};

struct DENGINE_API DirectionalShadowMap
{
	DirectionalShadowMap(uint Size = 4096);
	~DirectionalShadowMap();

	void Bind();
	void Unbind();

	uint m_FrameBuffer = 0;
	uint m_DepthTexture = 0;
	vec2d m_DepthTextureSize;
};

struct DENGINE_API DirectionalLight
{
	vec3d Direction;
	vec3d Position;
	color3 Radiance;

	float SourceLength = 1000.f;
	float ShadowMapInfuenceSize = 1000.f;
	float NearPlane = 10.f;
	float FarPlane = 10000.f;

	bool CastShadows = true;
	Ref<DirectionalShadowMap> ShadowMap;
	glm::mat4 ViewProjectionMatrix;
	vec3d LastDirection; // for caching
	vec3d LastPosition; // for caching
};

struct DENGINE_API CookedDirectionalLight
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

	void SubmitDirectionalLight(Ref<DirectionalLight> Light);
	void RemoveDirectionalLight(Ref<DirectionalLight> Light);

	void GenDrawCalls();
	std::vector<MeshDrawCall> CreateShadowMapDrawCalls();
	void RenderShadowMaps();


	MeshRendererSettings& GetSettingsMutable()
	{
		return m_Settings;
	}

private:

	//render settings
	MeshRendererSettings m_Settings;

	//draw calls for rendering meshes 
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
	std::vector<Ref<DirectionalLight>> m_DirectionalLights;
	std::vector<CookedDirectionalLight> m_CookedDirectionalLights;

	//shadow stuff
	const uint m_MaxDirectionalShadowMaps = 4;
};

