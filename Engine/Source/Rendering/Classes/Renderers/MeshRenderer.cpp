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

	//light
	m_DirectionalLights.push_back({{1,0,0,0}, {2, 2, 2, 2}});
}

void MeshRenderer::GenDrawCalls()
{
	//dynamic meshes each need a seperate draw call
	for (auto& mesh : m_MeshBuffer)
	{
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
		if(!mesh->IsValid()) continue;

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
		if(mesh->IsFlaggedChanged())
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
}

void MeshRenderer::PrepareFrame()
{
	Super::PrepareFrame();

	GenDrawCalls();
}

void MeshRenderer::RenderFrame(Ref<Camera> camera)
{
	Super::RenderFrame(camera);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//light data (common across draw calls and all meshes)
	m_LightDataBuffer->Bind();
	m_LightDataBuffer->SetData(m_DirectionalLights.data(), m_DirectionalLights.size() * sizeof(DirectionalLight));

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

		//directional light amount
		call.CommonMaterial->GetShader()->SetUniform1i("u_DirLightsCount", m_DirectionalLights.size());

		//cubemap stuff
		if (auto cubemapRenderer = GetPipeline()->GetRenderer<CubemapRenderer>())
		{
			if (auto skybox = cubemapRenderer->GetActiveCubemap())
			{
				glActiveTexture(GL_TEXTURE0 + 32); //irradiance in slot 32
				glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->m_IrradianceMap);
				call.CommonMaterial->GetShader()->SetUniform1i("u_CubemapIrradiance", 32);

				glActiveTexture(GL_TEXTURE0 + 31); //specular in slot 31
				glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->m_CubeMapSlot);
				call.CommonMaterial->GetShader()->SetUniform1i("u_CubemapSpecular", 31);

				glActiveTexture(GL_TEXTURE0 + 30); //BRDF in slot 30
				glBindTexture(GL_TEXTURE_2D, skybox->m_brdfMap);
				call.CommonMaterial->GetShader()->SetUniform1i("u_BRDF", 30);
			}
		}

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
}

void MeshRenderer::SubmitMesh(Ref<Mesh> mesh)
{
	m_MeshBuffer.push_back(mesh);
	mesh->AssignVertexBufferLayout(m_MeshVertexBufferLayout);
}

void MeshRenderer::RemoveMesh(Ref<Mesh> mesh)
{
	m_MeshBuffer.erase(std::find(m_MeshBuffer.begin(), m_MeshBuffer.end(), mesh));
}

void MeshRenderer::SubmitStaticMesh(Ref<StaticMesh> mesh)
{
	m_StaticMeshBuffer.push_back(mesh);
	//dont forget to asign the vertex buffer layout!
}

void MeshRenderer::RemoveStaticMesh(Ref<StaticMesh> mesh)
{
	m_StaticMeshBuffer.erase(std::find(m_StaticMeshBuffer.begin(), m_StaticMeshBuffer.end(), mesh));
}

