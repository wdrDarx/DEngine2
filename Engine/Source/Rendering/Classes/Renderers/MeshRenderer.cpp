#include "MeshRenderer.h"
#include "DEngine.h"

void MeshRenderer::OnConstruct()
{
	m_MeshDataBuffer = MakeRef<ShaderStorageBuffer>(0); //binding = 0
	m_LightDataBuffer = MakeRef<ShaderStorageBuffer>(1); //binding = 1

	m_MeshVertexBufferLayout = MakeRef<VertexBufferLayout>();
	m_MeshVertexBufferLayout->Push<float>(3); //vertex pos
	m_MeshVertexBufferLayout->Push<float>(3); //Vertex normal
	m_MeshVertexBufferLayout->Push<float>(3); //Vertex tangent
	m_MeshVertexBufferLayout->Push<float>(3); //Vertex bitangent
	m_MeshVertexBufferLayout->Push<float>(2); //texture coords
}

void MeshRenderer::GenDrawCalls()
{
	//dynamic meshes each need a seperate draw call
	for (auto& mesh : m_MeshBuffer)
	{
		if(mesh && mesh->GetMeshFlags() & MeshFlags::INVISIBLE) //no occlusion test because the mesh can change and bounds have to be recalculated too 
			continue;

		MeshDrawCall call;
		call.CommonMesh = mesh;
		call.MeshDataArray.push_back({mesh->GetMatrix()});
		call.CommonMaterial = mesh->GetMaterial();
		m_DrawCalls.push_back(call);
	}
	
	//for organization
	struct Group
	{
		bool needsReload = false;
		Ref<StaticMesh> CommonMesh;
		std::vector<MeshData> data;
	};

	//occlusion test for static meshes
	std::vector<Ref<StaticMesh>> VisibleMeshes;
	VisibleMeshes.reserve(m_StaticMeshBuffer.size());
	VisibleMeshes = MassOcclusionTest(m_StaticMeshBuffer);

	//static meshes get grouped into draw calls if they have the same material and mesh
	std::vector<Group> groups;
	for (auto& mesh : VisibleMeshes)
	{
		if(!mesh) continue;
		if(mesh && !mesh->IsValid() || mesh->GetMeshFlags() & MeshFlags::INVISIBLE) continue; //ignore

		bool fits = false;
		for (auto& group : groups)
		{
			if(!group.CommonMesh) continue;

			//check if we fit into this group
			if (mesh->m_MaterialAsset == group.CommonMesh->m_MaterialAsset &&
				mesh->m_MeshAsset == group.CommonMesh->m_MeshAsset)
			{
				//we fit
				if(mesh->IsFlaggedChanged())
					group.needsReload = true;

				//only push back the matrix since the group shares meshes
				group.data.push_back({mesh->GetMatrix()});
				fits = true;
				break;
			}

		}
		if(fits) continue; //othsewise it will create a group

		//none of the groups fit so make a new one with ourselves in it
		Group newGroup;
		newGroup.CommonMesh = mesh;
		newGroup.data.push_back({mesh->GetMatrix()});
		if(mesh->IsFlaggedChanged() || !mesh->GetLoadedMaterialAsset() || !mesh->GetLoadedMeshAsset()) //regen if flagged changed or no assets are load
			newGroup.needsReload = true;
		groups.push_back(newGroup);
	}

	//and now convert groups to draw calls
	for (auto& group : groups)
	{	
		//reload if any of the meshes in the group want a reload, only care about mesh[0] because they are all legit the same
		if(group.needsReload)
			group.CommonMesh->ReloadFromAssets(GetPipeline()->GetScene());

		//vertex array wasnt assigned yet
		if(!group.CommonMesh->GetVertexArray()->m_VertexBuffer)
			group.CommonMesh->AssignVertexBufferLayout(m_MeshVertexBufferLayout);

		//construct the draw call and push it back
		MeshDrawCall call;
		call.CommonMesh = group.CommonMesh;
		call.MeshDataArray = group.data;
		call.CommonMaterial = group.CommonMesh->GetMaterial();
		m_DrawCalls.push_back(call);
	}

	//cook refs to directional lights
	for (auto& light : m_DirectionalLights)
	{
		color4 dir = color4(light->Direction, 1.0);
		color4 radiance = color4(light->Radiance, 1.0);
		color4 LightSize = color4(light->LightSize);
		color4 MinMaxSoftness = color4(light->MinSoftness, light->MaxSoftness, 0.f, 0.f);
		m_CookedDirectionalLights.push_back({dir, radiance, LightSize, MinMaxSoftness});
	}
}

std::vector<MeshDrawCall> MeshRenderer::CreateShadowMapDrawCalls()
{
	std::vector<MeshDrawCall> out;

	//dynamic meshes each need a seperate draw call
	for (auto& mesh : m_MeshBuffer)
	{
		if ((mesh->GetMeshFlags() & MeshFlags::NOSHADOW) || (mesh->GetMeshFlags() & MeshFlags::INVISIBLE))
			continue;

		MeshDrawCall call;
		call.CommonMesh = mesh;
		call.MeshDataArray.push_back({ mesh->GetMatrix() });
		out.push_back(call);
	}

	//for organization
	struct Group
	{
		bool needsReload = false;
		Ref<StaticMesh> CommonMesh;
		std::vector<MeshData> data;
	};

	//static meshes get grouped into draw calls if they have the same material and mesh
	std::vector<Group> groups;
	for (auto& mesh : m_StaticMeshBuffer)
	{
		if (!mesh->IsValid() || (mesh->GetMeshFlags() & MeshFlags::INVISIBLE) || (mesh->GetMeshFlags() & MeshFlags::NOSHADOW)) continue;

		bool fits = false;
		for (auto& group : groups)
		{
			if (!group.CommonMesh) continue;

			//check if we fit into this group
			if (mesh->m_MeshAsset == group.CommonMesh->m_MeshAsset)
			{
				//we fit
				if (mesh->IsFlaggedChanged())
					group.needsReload = true;

				//only push back the matrix since the group shares meshes
				group.data.push_back({ mesh->GetMatrix() });
				fits = true;
				break;
			}

		}
		if (fits) continue; //othsewise it will create a group

		//none of the groups fit so make a new one with ourselves in it
		Group newGroup;
		newGroup.CommonMesh = mesh;
		newGroup.data.push_back({ mesh->GetMatrix() });
		if (mesh->IsFlaggedChanged())
			newGroup.needsReload = true;
		groups.push_back(newGroup);
	}

	//and now convert groups to draw calls
	for (auto& group : groups)
	{
		//reload if any of the meshes in the group want a reload, only care about mesh[0] because they are all legit the same
		if (group.needsReload)
			group.CommonMesh->ReloadFromAssets(GetPipeline()->GetScene());

		//vertex array wasnt assigned yet
		if (!group.CommonMesh->GetVertexArray()->m_VertexBuffer)
			group.CommonMesh->AssignVertexBufferLayout(m_MeshVertexBufferLayout);

		//construct the draw call and push it back
		MeshDrawCall call;
		call.CommonMesh = group.CommonMesh;
		call.MeshDataArray = group.data;
		call.CommonMaterial = group.CommonMesh->GetMaterial();
		out.push_back(call);
	}

	return out;
}

void MeshRenderer::RenderShadowMaps(Ref<Camera> camera, const std::vector<MeshDrawCall>& DrawCalls)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Ref<Shader> ShadowMapShader = GetPipeline()->GetRenderAPI()->GetShaderFromCache("ShadowMapShader");
	ShadowMapShader->Bind();

	auto& light = m_DirectionalLights[0]; //only the first light casts shadows
	if (!light->ShadowMap && !light->CastShadows) //remove shadow map
	{
		light->ShadowMap = nullptr;
	}

	if (!light->ShadowMap && light->CastShadows) //create shadow map
	{
		light->ShadowMap = MakeRef<DirectionalShadowMap>(light->NearPlane, light->FarPlane);
	}

	//bind light shadow map, set viewport size and clear the depth

	GLint CurrentFB = 0; //we should bind this after drawing all the shadow maps to resume drawing to whatever the texture target is
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &CurrentFB);

	vec2d CurrentViewport; //save the viewport
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	CurrentViewport.x = viewport[2];
	CurrentViewport.y = viewport[3];

	for (uint j = 0; j < light->ShadowMap->m_CascadeCount; j++)
	{
		light->ShadowMap->BindWrite(j);

		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, light->ShadowMap->m_DepthTextureSize.x, light->ShadowMap->m_DepthTextureSize.y);

		for (uint i = 0; i < DrawCalls.size(); i++)
		{
			auto& call = DrawCalls[i];
			if (!call.CommonMesh->IsValidForDraw()) continue; //invalid for draw

			//bind just the vertex array because we are using a custom shader
			call.CommonMesh->GetVertexArray()->Bind();

			//mesh data
			m_MeshDataBuffer->SetData(call.MeshDataArray.data(), call.MeshDataArray.size() * sizeof(MeshData));

			//set the projection matrix
			ShadowMapShader->SetUniformMat4f("u_ViewProjectionMatrix", light->ShadowMap->m_CascadeProjections[j]);

			//draw call
			GetPipeline()->GetRenderAPI()->DrawInstanced(*call.CommonMesh->GetIndexBuffer(), call.MeshDataArray.size());
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, CurrentFB);
	camera->GetRenderAPI()->SetViewport(CurrentViewport);
	//camera->RecalculateViewProjectionMatrix();

	ShadowMapShader->Unbind();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
}

void MeshRenderer::PrepareFrame()
{
	Super::PrepareFrame();

	if(!GetPipeline()->GetRenderAPI()->IsShaderInCache("ShadowMapShader"))
		GetPipeline()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\ShadowMap.shader"), "ShadowMapShader");

	if (!GetPipeline()->GetRenderAPI()->IsShaderInCache("BasicShader"))
		GetPipeline()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\BasicShader.shader"), "BasicShader");

	if (!GetPipeline()->GetRenderAPI()->IsShaderInCache("OcclusionCompute"))
		GetPipeline()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\Compute.shader"), "OcclusionCompute");

	GenDrawCalls();
	m_ShadowMeshDrawCalls = CreateShadowMapDrawCalls();

}

void MeshRenderer::RenderFrame(Ref<Camera> camera)
{
	Super::RenderFrame(camera);

	//set some values
	m_LastCamera = camera.get();

	if(m_DeferredFrameTarget)
	{
		RenderDeffered(camera, m_DeferredFrameTarget);
		return;
	}

	//gen cascades and render shadow maps for the first dir light
	if(m_DirectionalLights.size() > 0 && m_DirectionalLights[0]->CastShadows)
	{ 
		m_DirectionalLights[0]->GenCascades(camera.get());
		RenderShadowMaps(camera, m_ShadowMeshDrawCalls);
	}

// 	if(m_DirectionalLights.size() > 0)
// 	{ 
// 		auto shader = GetPipeline()->GetRenderAPI()->GetShaderFromCache("BasicShader");
// 		shader->Bind();
// 		shader->SetUniform1i("u_Texture", 0);
// 		glActiveTexture(GL_TEXTURE0);
// 		glBindTexture(GL_TEXTURE_2D, m_DirectionalLights[0]->ShadowMap->m_DepthTexture[0]);
// 		RenderUtils::RenderQuad();
// 		return;
// 	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//light data (common across draw calls and all meshes)
	m_LightDataBuffer->Bind();
	m_LightDataBuffer->SetData(m_CookedDirectionalLights.data(), m_CookedDirectionalLights.size() * sizeof(DirectionalLight));

	//cubemap stuff
	if (auto cubemapRenderer = GetPipeline()->GetRenderer<CubemapRenderer>())
	{
		if (auto skybox = cubemapRenderer->GetActiveCubemap())
		{
			glActiveTexture(GL_TEXTURE0 + 32); //irradiance in slot 32
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->m_IrradianceMap);
			
			glActiveTexture(GL_TEXTURE0 + 31); //specular in slot 31
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->m_CubeMapSlot);
			
			glActiveTexture(GL_TEXTURE0 + 30); //BRDF in slot 30
			glBindTexture(GL_TEXTURE_2D, skybox->m_brdfMap);
			
		}
	}

	std::vector<glm::mat4> DirectionLightMatricies;
	std::vector<float> DirectionalLightCascadeSplits;
	int DirectionalShadowMapSlots[4] = {0,0,0,0};
	uint DirectionalLightShadowMapAmount = 0;
	if(m_DirectionalLights.size() > 0)
	{ 
		auto& light = m_DirectionalLights[0];
		if (light->ShadowMap && light->CastShadows)
		{
			for(uint j = 0; j < light->ShadowMap->m_CascadeCount; j++)
			{ 
				DirectionLightMatricies.push_back(light->ShadowMap->m_CascadeProjections[j]);
				DirectionalShadowMapSlots[j] = 29 - j; //start at 29, and go to 26
				glActiveTexture(GL_TEXTURE0 + DirectionalShadowMapSlots[j]);
				glBindTexture(GL_TEXTURE_2D, light->ShadowMap->m_DepthTexture[j]);
				DirectionalLightShadowMapAmount++;
			}
			DirectionalLightCascadeSplits = light->ShadowMap->m_CascadesSplit;
		}
	}

	for(uint i = 0; i < m_DrawCalls.size() ; i++)
	{
		auto& call = m_DrawCalls[i];
		if(!call.CommonMesh->IsValidForDraw()) continue; //invalid for draw

		call.CommonMesh->Bind(GetPipeline()->GetRenderAPI()); //binds mesh vertex array and material
		if(!call.CommonMaterial->GetShader()) continue; //shader not valid

		//mesh data
		m_MeshDataBuffer->SetData(call.MeshDataArray.data(), call.MeshDataArray.size() * sizeof(MeshData));

		//camera pos uniform
		vec3d cameraPos = camera->GetTransform().pos;
		call.CommonMaterial->GetShader()->SetUniform3f("u_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
		
		//view projection matrix
		call.CommonMaterial->GetShader()->SetUniformMat4f("u_ViewProjectionMatrix", camera->GetViewProjectionMatrix());
		call.CommonMaterial->GetShader()->SetUniformMat4f("u_ViewMatrix", camera->GetViewMatrix());

		//ambient light multiplier
		call.CommonMaterial->GetShader()->SetUniform1f("u_AmbientLightMult", GetSettingsMutable().AmbientLightMultiplier);

		//cubemap
		call.CommonMaterial->GetShader()->SetUniform1i("u_CubemapIrradiance", 32);
		call.CommonMaterial->GetShader()->SetUniform1i("u_CubemapSpecular", 31);
		call.CommonMaterial->GetShader()->SetUniform1i("u_BRDF", 30);

		//directional light amount
		call.CommonMaterial->GetShader()->SetUniform1i("u_DirLightsCount", m_CookedDirectionalLights.size());

		//directional light shadow maps
		call.CommonMaterial->GetShader()->SetUniformMat4fv("u_DirectionLightMatricies", DirectionalLightShadowMapAmount, (float*)DirectionLightMatricies.data());
		call.CommonMaterial->GetShader()->SetUniform1iv("u_DirectionalShadowMaps", DirectionalLightShadowMapAmount, DirectionalShadowMapSlots);
		call.CommonMaterial->GetShader()->SetUniform1fv("u_CascadeEndClipSpace", DirectionalLightShadowMapAmount, DirectionalLightCascadeSplits.data());
		call.CommonMaterial->GetShader()->SetUniform1i("u_DirectionalShadowMapCount", DirectionalLightShadowMapAmount);

		//draw call
		GetPipeline()->GetRenderAPI()->DrawInstanced(*call.CommonMesh->GetIndexBuffer(), call.MeshDataArray.size());
	}
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
}

void MeshRenderer::ClearFrame()
{
	Super::ClearFrame();

	m_DrawCalls.clear();
	m_ShadowMeshDrawCalls.clear();
	m_CookedDirectionalLights.clear();
}

void MeshRenderer::SubmitMesh(Ref<Mesh> mesh)
{
	m_MeshBuffer.push_back(mesh);
	mesh->AssignVertexBufferLayout(m_MeshVertexBufferLayout);
}

void MeshRenderer::RemoveMesh(Ref<Mesh> mesh)
{
	ObjectUtils::RemoveFromVector(mesh, m_MeshBuffer);
}

void MeshRenderer::SubmitStaticMesh(Ref<StaticMesh> mesh)
{
	m_StaticMeshBuffer.push_back(mesh);
	//dont forget to asign the vertex buffer layout!
}

void MeshRenderer::RemoveStaticMesh(Ref<StaticMesh> mesh)
{
	ObjectUtils::RemoveFromVector(mesh, m_StaticMeshBuffer);
}

void MeshRenderer::SubmitDirectionalLight(Ref<DirectionalLight> Light)
{
	m_DirectionalLights.push_back(Light);
}

void MeshRenderer::RemoveDirectionalLight(Ref<DirectionalLight> Light)
{
	ObjectUtils::RemoveFromVector(Light, m_DirectionalLights);
}


bool MeshRenderer::OcclusionTest(Ref<Mesh> mesh)
{
	return true;
}

std::vector<Ref<StaticMesh>> MeshRenderer::MassOcclusionTest(const std::vector<Ref<StaticMesh>>& inputMeshes)
{
	if (!GetSettingsMutable().OcclusionCulling || inputMeshes.size() < 1) return inputMeshes;

	std::vector<Ref<StaticMesh>> out;
	out.resize(inputMeshes.size());

	uint ThreadCount = m_JobPool.numThreads;
	uint JobsPerThread = ceil((float)inputMeshes.size() / (float)ThreadCount);

	std::mutex mu;
	uint t_JobIndex = 0;
	for(uint i = 0; i < ThreadCount; i++)
	{
		m_JobPool.Execute([&]()
		{
			uint StartIndex = 0;	
			{
				std::lock_guard<std::mutex> lock(mu);
				StartIndex = t_JobIndex;
				//LogTemp("Index " + STRING(index));
				t_JobIndex += JobsPerThread;				
			}

			uint EndIndex = StartIndex + JobsPerThread;
			 
			for(uint index = 0; index <= EndIndex; index++)
			{
				if(index >= inputMeshes.size()) 
					return;

				if(!inputMeshes[index]->GetLoadedMeshAsset())
				{
					out[index] = inputMeshes[index];
					continue;
				}

				auto bounds = inputMeshes[index]->GetCachedBounds();
				vec3d testMin = bounds.min;
				vec3d testMax = bounds.max;

				if(true)
				{
					std::lock_guard<std::mutex> lock(mu);			
					GetPipeline()->GetRenderer<DebugRenderer>()->DrawDebugCube(testMin, {0,0,0}, {10,10,10}, {0,0,1});
					GetPipeline()->GetRenderer<DebugRenderer>()->DrawDebugCube(testMax, {0,0,0}, {10,10,10}, {0,0,1});
				}

				auto FrustrumNormals = m_LastCamera->GetFrustrumPlaneNormals();

				bool Cull = false;
				for (uint i = 0; i < 6; i++)
				{
					vec3d planeNormal = vec3d(FrustrumNormals[i].x, FrustrumNormals[i].y, FrustrumNormals[i].z);
					float planeConstant = FrustrumNormals[i].w;
					vec3d axisVert;

					if (FrustrumNormals[i].x < 0.0f)
						axisVert.x = testMin.x;
					else
						axisVert.x = testMax.x;

					if (FrustrumNormals[i].y < 0.0f) 
						axisVert.y = testMin.y;
					else
						axisVert.y = testMax.y;

					if (FrustrumNormals[i].z < 0.0f)    
						axisVert.z = testMin.z;
					else
						axisVert.z = testMax.z;

					if ( glm::dot(planeNormal, axisVert) + planeConstant < 0.0f)
					{
						Cull = true;
						break;
					}
				}

				if(!Cull)
				{
					out[index] = inputMeshes[index];
				}
			}
		});
	}

	m_JobPool.Wait();
	//LogTemp("\n");

	return out;
}

DirectionalShadowMap::DirectionalShadowMap(float NearPlane, float FarPlane, uint Size /*= 1024*/) : m_DepthTextureSize(Size, Size)
{
	glGenFramebuffers(1, &m_FrameBuffer);

	for(uint i = 0; i < m_CascadeCount; i++)
	{ 
		glGenTextures(1, &m_DepthTexture[i]);
		glBindTexture(GL_TEXTURE_2D, m_DepthTexture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, Size, Size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		float clampColor[] = {1,1,1,1}; 
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture[0], 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DirectionalShadowMap::~DirectionalShadowMap()
{
	for (auto texture : m_DepthTexture)
	{
		glDeleteTextures(1, &texture);
	}	

	glDeleteFramebuffers(1, &m_FrameBuffer);
}

void DirectionalShadowMap::BindWrite(uint CascadeIndex)
{
	ASSERT(CascadeIndex < m_CascadeCount);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture[CascadeIndex], 0);
}

void DirectionalShadowMap::BindRead(uint StartTextureSlot, uint NumberOfCascadesToBind)
{
	ASSERT(NumberOfCascadesToBind <= m_CascadeCount);
	for (uint i = 0; i < NumberOfCascadesToBind; i++)
	{
		uint slot = StartTextureSlot + i;
		ASSERT(slot <= 32); //already too many textures bound

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_DepthTexture[i]);
	}
}

void DirectionalShadowMap::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DirectionalShadowMap::GenCascadeProjections(Camera* camera, DirectionalLight* DirLight)
{
#if 0
	// Get the inverse of the view transform
	glm::mat4 CamInv = glm::inverse(camera->GetViewMatrix());

	float ar = camera->GetRenderAPI()->GetViewportSize().x / camera->GetRenderAPI()->GetViewportSize().y;
	float tanHalfHFOV = tanf(glm::radians(camera->getFOV() / 2.0f));
	float tanHalfVFOV = tanf(glm::radians((camera->getFOV() * ar) / 2.0f));

	for (uint i = 0; i < m_CascadeCount; i++)
	{
		float xn = m_CascadeEnd[i] * tanHalfHFOV;
		float xf = m_CascadeEnd[i + 1] * tanHalfHFOV;
		float yn = m_CascadeEnd[i] * tanHalfVFOV;
		float yf = m_CascadeEnd[i + 1] * tanHalfVFOV;

		//8 frustum corners
		vec4d frustumCorners[8] = 
		{
			// near face
			vec4d(xn, yn, m_CascadeEnd[i], 1.0),
			vec4d(-xn, yn, m_CascadeEnd[i], 1.0),
			vec4d(xn, -yn, m_CascadeEnd[i], 1.0),
			vec4d(-xn, -yn, m_CascadeEnd[i], 1.0),

			// far face
			vec4d(xf, yf, m_CascadeEnd[i + 1], 1.0),
			vec4d(-xf, yf, m_CascadeEnd[i + 1], 1.0),
			vec4d(xf, -yf, m_CascadeEnd[i + 1], 1.0),
			vec4d(-xf, -yf, m_CascadeEnd[i + 1], 1.0)
		};

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::min();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::min();

		for (uint j = 0; j < 8; j++)
		{

			// Transform the frustum coordinate from view to world space
			vec4d vW = CamInv * frustumCorners[j];

			// Transform the frustum coordinate from world to light space
			frustumCorners[j] = LightView * vW;

			minX = std::min(minX, frustumCorners[j].x);
			maxX = std::max(maxX, frustumCorners[j].x);
			minY = std::min(minY, frustumCorners[j].y);
			maxY = std::max(maxY, frustumCorners[j].y);
			minZ = std::min(minZ, frustumCorners[j].z);
			maxZ = std::max(maxZ, frustumCorners[j].z);
		}

		//m_CascadeProjections[i] = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
		m_CascadeProjections[i] = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ) * LightView;
	}

#else
	glm::mat4 viewProjection = camera->GetViewProjectionMatrix();

	const int SHADOW_MAP_CASCADE_COUNT = 4;
	float cascadeSplits[SHADOW_MAP_CASCADE_COUNT];

	// TODO: less hard-coding!
	float nearClip = DirLight->NearPlane;
	float farClip = DirLight->FarPlane;
	float clipRange = farClip - nearClip;

	float CascadeSplitLambda = 0.72f;
	float CascadeFarPlaneOffset = 5000.f, CascadeNearPlaneOffset = -5000.0f;
	//float CascadeFarPlaneOffset = 0.f, CascadeNearPlaneOffset = 0.f;

	float minZ = nearClip;
	float maxZ = nearClip + clipRange;

	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	//CascadeFarPlaneOffset = range / -2.0f;
	//CascadeNearPlaneOffset = range / 2.0f;

	// Calculate split depths based on view camera frustum
	// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
		float log = minZ * std::pow(ratio, p);
		float uniform = minZ + range * p;
		float d = CascadeSplitLambda * (log - uniform) + uniform;
		cascadeSplits[i] = (d - nearClip) / clipRange;
	}

	// cascadeSplits[0] = nearClip;
	 //cascadeSplits[1] = 0.1f;
	// cascadeSplits[2] = 0.4f;
	//cascadeSplits[3] = 0.3f;

	// Calculate orthographic projection matrix for each cascade
	float lastSplitDist = 0.0;
	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		float splitDist = cascadeSplits[i];

		glm::vec3 frustumCorners[8] =
		{
			glm::vec3(-1.0f,  1.0f, -1.0f),
			glm::vec3(1.0f,  1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f,  1.0f,  1.0f),
			glm::vec3(1.0f,  1.0f,  1.0f),
			glm::vec3(1.0f, -1.0f,  1.0f),
			glm::vec3(-1.0f, -1.0f,  1.0f),
		};

		// Project frustum corners into world space
		glm::mat4 invCam = glm::inverse(viewProjection);
		for (uint32_t i = 0; i < 8; i++)
		{
			glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
			frustumCorners[i] = invCorner / invCorner.w;
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
			frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
			frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
		}

		// Get frustum center
		glm::vec3 frustumCenter = glm::vec3(0.0f);
		for (uint32_t i = 0; i < 8; i++)
			frustumCenter += frustumCorners[i];

		frustumCenter /= 8.0f;

		//frustumCenter *= 0.01f;

		float radius = 0.0f;
		for (uint32_t i = 0; i < 8; i++)
		{
			float distance = glm::length(frustumCorners[i] - frustumCenter);
			radius = glm::max(radius, distance);
		}
		radius = std::ceil(radius * 16.0f) / 16.0f;

		glm::vec3 maxExtents = glm::vec3(radius);
		glm::vec3 minExtents = -maxExtents;

		glm::vec3 lightDir = DirLight->Direction;
		glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f + CascadeNearPlaneOffset, maxExtents.z - minExtents.z + CascadeFarPlaneOffset);

		// Offset to texel space to avoid shimmering (from https://stackoverflow.com/questions/33499053/cascaded-shadow-map-shimmering)
		glm::mat4 shadowMatrix = lightOrthoMatrix * lightViewMatrix;
		const float ShadowMapResolution = m_DepthTextureSize.x;
		glm::vec4 shadowOrigin = (shadowMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) * ShadowMapResolution / 2.0f;
		glm::vec4 roundedOrigin = glm::round(shadowOrigin);
		glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
		roundOffset = roundOffset * 2.0f / ShadowMapResolution;
		roundOffset.z = 0.0f;
		roundOffset.w = 0.0f;

		lightOrthoMatrix[3] += roundOffset;

		// Store split distance and matrix in cascade
		m_CascadesSplit[i] = (nearClip + splitDist * clipRange);
		m_CascadeProjections[i] = lightOrthoMatrix * lightViewMatrix;

		lastSplitDist = cascadeSplits[i];
	}

#endif
}

void DirectionalLight::GenCascades(Camera* camera)
{
	if(ShadowMap)
		ShadowMap->GenCascadeProjections(camera, this);
}

void DeferredFrame::GenTextures(const vec2d& resolution, const FrameBufferSpec& FrameSpec)
{
	if(resolution == Resolution) return;
	Resolution = resolution;

	//save the render target frame buffer (can be 0 too which means no render target)
	GLint DrawFrameBuffer = 0;
	GLint ReadFrameBuffer = 0;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &DrawFrameBuffer);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &ReadFrameBuffer);

	FrameBufferSpec gSpec;
	gSpec.Samples = 1;
	gSpec.Width = resolution.x;
	gSpec.Height = resolution.y;

	ColorAttachmentSpec WorldPosSpec;
	WorldPosSpec.Channels = GL_RGBA;
	WorldPosSpec.ColorFormat = GL_RGBA32F;
	WorldPosSpec.ColorDataType = GL_FLOAT;
	WorldPosSpec.FilterMode = GL_NEAREST;

	ColorAttachmentSpec WorldNormalSpec;
	WorldNormalSpec.Channels = GL_RGBA;
	WorldNormalSpec.ColorFormat = GL_RGBA16F;
	WorldNormalSpec.ColorDataType = GL_FLOAT;
	WorldNormalSpec.FilterMode = GL_NEAREST;

	ColorAttachmentSpec AlbedoSpec;
	AlbedoSpec.Channels = GL_RGBA;
	AlbedoSpec.ColorFormat = GL_RGBA16F;
	AlbedoSpec.ColorDataType = GL_UNSIGNED_BYTE;
	AlbedoSpec.FilterMode = GL_NEAREST;

	ColorAttachmentSpec AO_R_M_Spec;
	AO_R_M_Spec.Channels = GL_RGBA;
	AO_R_M_Spec.ColorFormat = GL_RGBA16F;
	AO_R_M_Spec.ColorDataType = GL_UNSIGNED_BYTE;
	AO_R_M_Spec.FilterMode = GL_NEAREST;

	gSpec.AttachColor(0, WorldPosSpec);
	gSpec.AttachColor(1, WorldNormalSpec);
	gSpec.AttachColor(2, AlbedoSpec);
	gSpec.AttachColor(3, AO_R_M_Spec);

	gBuffer = MakeRef<FrameBuffer>(gSpec);

#if 0
	//bind deferred framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, gBufferFrame);

	// - position color buffer
	glGenTextures(1, &WorldPos);
	glBindTexture(GL_TEXTURE_2D, WorldPos);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, WorldPos, 0);

	// - normal color buffer
	glGenTextures(1, &WorldNormal);
	glBindTexture(GL_TEXTURE_2D, WorldNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, WorldNormal, 0);

	// - albedo color buffer
	glGenTextures(1, &Albedo);
	glBindTexture(GL_TEXTURE_2D, Albedo);
	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, Albedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_RGBA32F, resolution.x, resolution.y, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, Albedo, 0);

	// - AO roughness metallic color buffer
	glGenTextures(1, &AO_Roughness_Metallic);
	glBindTexture(GL_TEXTURE_2D, AO_Roughness_Metallic);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, AO_Roughness_Metallic, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	uint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,  GL_COLOR_ATTACHMENT3};
	glDrawBuffers(4, attachments);

	//depth renderbuffer
	glGenRenderbuffers(1, &Depth);
	glBindRenderbuffer(GL_RENDERBUFFER, Depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, resolution.x, resolution.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Depth);

	ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
#endif
	//bind previous frame buffers
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, DrawFrameBuffer);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ReadFrameBuffer);
}


void MeshRenderer::RenderDeffered(Ref<Camera> camera, Ref<DeferredFrame> frame)
{
	//save the render target frame buffer (can be 0 too which means no render target)
	GLint DrawFrameBuffer = 0;
	GLint ReadFrameBuffer = 0;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &DrawFrameBuffer);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &ReadFrameBuffer);

	vec2d CurrentViewport; //save the viewport
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	CurrentViewport.x = viewport[2];
	CurrentViewport.y = viewport[3];

	//copy all current depth into the gbuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ReadFrameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame->gBuffer->GetFrameBufferID());
	glBlitFramebuffer(0, 0, CurrentViewport.x, CurrentViewport.y, 0, 0, frame->Resolution.x, frame->Resolution.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	//bind the frame buffer, remember this has color attachments that will get written into by the shader
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame->gBuffer->GetFrameBufferID());

	camera->GetRenderAPI()->SetClearColor({0,0,0,0});
	camera->GetRenderAPI()->Clear();
	camera->GetRenderAPI()->SetViewport(frame->Resolution); //set the desired resolution of the frame

	//the PBR material auto detects deffered rendering and switches to the deffered shader
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (uint i = 0; i < m_DrawCalls.size(); i++)
	{
		auto& call = m_DrawCalls[i];
		if (!call.CommonMesh->IsValidForDraw()) continue; //invalid for draw

		call.CommonMesh->Bind(GetPipeline()->GetRenderAPI()); //binds mesh vertex array and material
		if (!call.CommonMaterial->GetShader()) continue; //shader not valid

		//mesh data
		m_MeshDataBuffer->SetData(call.MeshDataArray.data(), call.MeshDataArray.size() * sizeof(MeshData));

		//view projection matrix
		call.CommonMaterial->GetShader()->SetUniformMat4f("u_ViewProjectionMatrix", camera->GetViewProjectionMatrix());
	
		//set camera pos uniform
		vec3d cameraPos = camera->GetTransform().pos;
		call.CommonMaterial->GetShader()->SetUniform3f("u_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

		//draw call
		GetPipeline()->GetRenderAPI()->DrawInstanced(*call.CommonMesh->GetIndexBuffer(), call.MeshDataArray.size());
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	//COMPOSITE
	//--------

	//gen cascades and render shadow maps for the first dir light
	if (m_DirectionalLights.size() > 0 && m_DirectionalLights[0]->CastShadows)
	{
		m_DirectionalLights[0]->GenCascades(camera.get());
		RenderShadowMaps(camera, m_ShadowMeshDrawCalls);
	}

	//the gBuffer now has all the color attachments filled with relevant data, now composite them all and write to the final frame buffer
	if (!camera->GetRenderAPI()->IsShaderInCache("PBR_Composite"))
	{
		camera->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "\\Shaders\\PBR_Composite.shader"), "PBR_Composite");
	}


	//light data (common across draw calls and all meshes)
	m_LightDataBuffer->Bind();
	m_LightDataBuffer->SetData(m_CookedDirectionalLights.data(), m_CookedDirectionalLights.size() * sizeof(DirectionalLight));

	//cubemap stuff
	if (auto cubemapRenderer = GetPipeline()->GetRenderer<CubemapRenderer>())
	{
		if (auto skybox = cubemapRenderer->GetActiveCubemap())
		{
			glActiveTexture(GL_TEXTURE0 + 32); //irradiance in slot 32
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->m_IrradianceMap);

			glActiveTexture(GL_TEXTURE0 + 31); //specular in slot 31
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->m_CubeMapSlot);

			glActiveTexture(GL_TEXTURE0 + 30); //BRDF in slot 30
			glBindTexture(GL_TEXTURE_2D, skybox->m_brdfMap);

		}
	}

	std::vector<glm::mat4> DirectionLightMatricies;
	std::vector<float> DirectionalLightCascadeSplits;
	int DirectionalShadowMapSlots[4] = { 0,0,0,0 };
	uint DirectionalLightShadowMapAmount = 0;
	if (m_DirectionalLights.size() > 0)
	{
		auto& light = m_DirectionalLights[0];
		if (light->ShadowMap && light->CastShadows)
		{
			for (uint j = 0; j < light->ShadowMap->m_CascadeCount; j++)
			{
				DirectionLightMatricies.push_back(light->ShadowMap->m_CascadeProjections[j]);
				DirectionalShadowMapSlots[j] = 29 - j; //start at 29, and go to 26
				glActiveTexture(GL_TEXTURE0 + DirectionalShadowMapSlots[j]);
				glBindTexture(GL_TEXTURE_2D, light->ShadowMap->m_DepthTexture[j]);
				DirectionalLightShadowMapAmount++;
			}
			DirectionalLightCascadeSplits = light->ShadowMap->m_CascadesSplit;
		}
	}

	//bind gbuffer textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frame->gBuffer->GetColorAttachement(0)); //WorldPos

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, frame->gBuffer->GetColorAttachement(1)); //world normal

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, frame->gBuffer->GetColorAttachement(2)); //Albedo

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, frame->gBuffer->GetColorAttachement(3)); //AO_R_M

	//composite the gbuffers
	Ref<Shader> CompositeShader = camera->GetRenderAPI()->GetShaderFromCache("PBR_Composite");
	CompositeShader->Bind();
	CompositeShader->SetUniform1i("u_Position", 0);
	CompositeShader->SetUniform1i("u_Normal", 1);
	CompositeShader->SetUniform1i("u_Albedo", 2);
	CompositeShader->SetUniform1i("u_AO_Roughness_Metallic", 3);

	//camera pos uniform
	vec3d cameraPos = camera->GetTransform().pos;
	CompositeShader->SetUniform3f("u_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

	//view projection matrix
	CompositeShader->SetUniformMat4f("u_ViewProjectionMatrix", camera->GetViewProjectionMatrix());
	CompositeShader->SetUniformMat4f("u_ViewMatrix", camera->GetViewMatrix());

	//ambient light multiplier
	CompositeShader->SetUniform1f("u_AmbientLightMult", GetSettingsMutable().AmbientLightMultiplier);

	//cubemap
	CompositeShader->SetUniform1i("u_CubemapIrradiance", 32);
	CompositeShader->SetUniform1i("u_CubemapSpecular", 31);
	CompositeShader->SetUniform1i("u_BRDF", 30);

	//directional light amount
	CompositeShader->SetUniform1i("u_DirLightsCount", m_CookedDirectionalLights.size());

	//directional light shadow maps
	CompositeShader->SetUniformMat4fv("u_DirectionalLightMatricies", DirectionalLightShadowMapAmount, (float*)DirectionLightMatricies.data());
	CompositeShader->SetUniform1iv("u_DirectionalShadowMaps", DirectionalLightShadowMapAmount, DirectionalShadowMapSlots);
	CompositeShader->SetUniform1fv("u_CascadeEndClipSpace", DirectionalLightShadowMapAmount, DirectionalLightCascadeSplits.data());
	CompositeShader->SetUniform1i("u_DirectionalShadowMapCount", DirectionalLightShadowMapAmount);
		
	//render to whatever render target was bound
	camera->GetRenderAPI()->SetViewport(CurrentViewport);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, DrawFrameBuffer);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ReadFrameBuffer);

	//render quad to target frame
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);
	RenderUtils::RenderQuad();
	glDepthMask(true);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, frame->gBuffer->GetFrameBufferID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, DrawFrameBuffer); // write to default framebuffer
	glBlitFramebuffer(0, 0, CurrentViewport.x, CurrentViewport.y, 0, 0, CurrentViewport.x, CurrentViewport.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, DrawFrameBuffer);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ReadFrameBuffer);
}

void MeshRenderer::SetRenderDeffered(Ref<DeferredFrame> frame)
{
	m_DeferredFrameTarget = frame;
}

void DeferredFrame::Destroy(bool TexturesOnly)
{
// 	glDeleteTextures(1, &WorldPos);
// 	glDeleteTextures(1, &WorldNormal);
// 	glDeleteTextures(1, &Albedo);
// 	glDeleteTextures(1, &AO_Roughness_Metallic);
// 
// 	if(!TexturesOnly)
// 	{
// 		//glDeleteFramebuffers(1, &gBufferFrame);
// 	}
	gBuffer = nullptr;
}
