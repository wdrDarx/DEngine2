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
		if(mesh->GetMeshFlags() & MeshFlags::INVISIBLE)
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

	//static meshes get grouped into draw calls if they have the same material and mesh
	std::vector<Group> groups;
	for (auto& mesh : m_StaticMeshBuffer)
	{
		if(!mesh->IsValid() || mesh->GetMeshFlags() & MeshFlags::INVISIBLE) continue;

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
		m_CookedDirectionalLights.push_back({dir, radiance});
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

void MeshRenderer::RenderShadowMaps()
{	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Ref<Shader> ShadowMapShader = GetPipeline()->GetRenderAPI()->GetShaderFromCache("ShadowMapShader");
	ShadowMapShader->Bind();

	for(auto& light : m_DirectionalLights)
	{
		if (!light->ShadowMap && !light->CastShadows) //remove shadow map
		{
			light->ShadowMap = nullptr;
			continue;
		}

		if (!light->ShadowMap && light->CastShadows) //create shadow map
		{
			light->ShadowMap = MakeRef<DirectionalShadowMap>();
		}

		if(light->LastDirection != light->Direction || light->Position != light->LastPosition) //need to regenerate matrix
		{ 
			glm::mat4 ProjectionMatrix = glm::ortho(-1.f * light->ShadowMapInfuenceSize, light->ShadowMapInfuenceSize, -1.f * light->ShadowMapInfuenceSize, light->ShadowMapInfuenceSize, light->NearPlane, light->FarPlane);
			glm::mat4 LightViewMatrix = glm::lookAt(light->Direction * -1.f * light->SourceLength + light->Position, light->Position, { 0,1,0 }); //light direction looking at the center + directional light location
			light->ViewProjectionMatrix = ProjectionMatrix * LightViewMatrix;
			light->LastDirection = light->Direction;
			light->LastPosition = light->Position;
		}

		//bind light shadow map, set viewport size and clear the depth
		light->ShadowMap->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, light->ShadowMap->m_DepthTextureSize.x, light->ShadowMap->m_DepthTextureSize.y);

		const std::vector<MeshDrawCall>& DrawCalls = CreateShadowMapDrawCalls();
		for (uint i = 0; i < DrawCalls.size(); i++)
		{
			auto& call = DrawCalls[i];
			if (!call.CommonMesh->IsValidForDraw()) continue; //invalid for draw

			//bind just the vertex array because we are using a custom shader
			call.CommonMesh->GetVertexArray()->Bind();

			//mesh data
			m_MeshDataBuffer->SetData(call.MeshDataArray.data(), call.MeshDataArray.size() * sizeof(MeshData));

			//view projection matrix (the lights matrix)
			ShadowMapShader->SetUniformMat4f("u_ViewProjectionMatrix", light->ViewProjectionMatrix);

			//draw call
			GetPipeline()->GetRenderAPI()->DrawInstanced(*call.CommonMesh->GetIndexBuffer(), call.MeshDataArray.size());
		}

		light->ShadowMap->Unbind();
	}
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

	GenDrawCalls();
	RenderShadowMaps();
}

void MeshRenderer::RenderFrame(Ref<Camera> camera)
{
	Super::RenderFrame(camera);

// 	if(m_DirectionalLights.size() > 0)
// 	{ 
// 		auto shader = GetPipeline()->GetRenderAPI()->GetShaderFromCache("BasicShader");
// 		shader->Bind();
// 		shader->SetUniform1i("u_Texture", 0);
// 		glActiveTexture(GL_TEXTURE0);
// 		glBindTexture(GL_TEXTURE_2D, m_DirectionalLights[0]->ShadowMap->m_DepthTexture);
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

	//directional lights shadow maps
	std::vector<glm::mat4> DirectionLightMatricies;
	int DirectionalShadowMapSlots[4];
	uint DirectionalLightShadowMapAmount = 0;
	for (uint i = 0; i < m_DirectionalLights.size(); i++)
	{
		if (i > m_MaxDirectionalShadowMaps) break; //dont exceed max shadow maps

		auto& light = m_DirectionalLights[i];
		if (light->ShadowMap && light->CastShadows)
		{
			DirectionLightMatricies.push_back(light->ViewProjectionMatrix);
			DirectionalShadowMapSlots[i] = 29 - i; //start at 29, and go to 25
			glActiveTexture(GL_TEXTURE0 + DirectionalShadowMapSlots[i]);
			glBindTexture(GL_TEXTURE_2D, light->ShadowMap->m_DepthTexture);
			DirectionalLightShadowMapAmount++;
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

DirectionalShadowMap::DirectionalShadowMap(uint Size /*= 1024*/) : m_DepthTextureSize(Size, Size)
{
	glGenFramebuffers(1, &m_FrameBuffer);

	glGenTextures(1, &m_DepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Size, Size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float clampColor[] = {1,1,1,1}; 
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DirectionalShadowMap::~DirectionalShadowMap()
{
	glDeleteTextures(1, &m_DepthTexture);
	glDeleteFramebuffers(1, &m_FrameBuffer);
}

void DirectionalShadowMap::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
}

void DirectionalShadowMap::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
