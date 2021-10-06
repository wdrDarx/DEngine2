#pragma once
#include "Rendering/RenderCore.h"
#include "Rendering/Renderer.h"
#include "Utils/Task.h"

struct DENGINE_API MeshRendererSettings
{
	float AmbientLightMultiplier = 0.5f;
	bool OcclusionCulling = false;
};


struct DirectionalLight;
struct DENGINE_API DirectionalShadowMap
{
	DirectionalShadowMap(float NearPlane, float FarPlane, uint Size = 4096);
	~DirectionalShadowMap();

	void BindWrite(uint CascadeIndex);

	void BindRead(uint StartTextureSlot, uint NumberOfCascadesToBind);

	void Unbind();

	void GenCascadeProjections(Camera* camera, DirectionalLight* DirLight);

	uint m_FrameBuffer = 0;
	std::vector<uint> m_DepthTexture = {0,0,0,0}; //4 cascades
	const uint m_CascadeCount = m_DepthTexture.size();

	std::vector<glm::mat4> m_CascadeProjections = std::vector<glm::mat4>(m_CascadeCount);
	std::vector<float> m_CascadesSplit = std::vector<float>(m_CascadeCount);

	vec2d m_DepthTextureSize;
};

struct DENGINE_API DirectionalLight
{
	//call when a camera is available
	void GenCascades(Camera* camera);
	
	//3d direction into which the light is pointing
	vec3d Direction;

	//color and intensity of the light
	color3 Radiance;

	float NearPlane = 0.01f;
	float FarPlane = 10000.f;

	//more = softer shadows
	float LightSize = 1000.f;

	float MinSoftness = 0.0008f;
	float MaxSoftness = 0.002f;

	bool CastShadows = true;
	Ref<DirectionalShadowMap> ShadowMap;
};

struct DENGINE_API CookedDirectionalLight
{
	color4 Direction;
	color4 Radiance;
	color4 LightSize;
	color4 MinMaxSoftness;
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

struct DENGINE_API DeferredFrame
{
	void GenTextures(const vec2d& resolution, const FrameBufferSpec& FrameSpec = FrameBufferSpec());
	void Destroy(bool TexturesOnly = false);

	~DeferredFrame()
	{
		Destroy();
	}

	vec2d Resolution = {0,0};
	Ref<FrameBuffer> gBuffer;
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

	bool OcclusionTest(Ref<Mesh> mesh);
	std::vector<Ref<StaticMesh>> MassOcclusionTest(const std::vector<Ref<StaticMesh>>& inputMeshes);

	//set to nullptr to disable
	void SetRenderDeffered(Ref<DeferredFrame> frame);

	void RenderDeffered(Ref<Camera> camera, Ref<DeferredFrame> frame);

	void GenDrawCalls();
	std::vector<MeshDrawCall> CreateShadowMapDrawCalls();

	void RenderShadowMaps(Ref<Camera> camera, const std::vector<MeshDrawCall>& DrawCalls);


	MeshRendererSettings& GetSettingsMutable()
	{
		return m_Settings;
	}

public:
	
	//if this is not nullptr, meshes will be rendered deferred
	Ref<DeferredFrame> m_DeferredFrameTarget = nullptr;

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

	//draw calls for rendering shadow maps 
	std::vector<MeshDrawCall> m_ShadowMeshDrawCalls;

	//used for occlusion tests
	Camera* m_LastCamera = nullptr;
	Transform m_LastCameraTransform;
	JobPool m_JobPool;
};

