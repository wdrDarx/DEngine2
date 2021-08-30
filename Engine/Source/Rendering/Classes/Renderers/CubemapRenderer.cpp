#include "CubemapRenderer.h"
#include "DEngine.h"

void CubemapRenderer::OnConstruct()
{

}

void CubemapRenderer::PrepareFrame()
{
	
}

void CubemapRenderer::RenderFrame(Ref<Camera> camera)
{
	Super::RenderFrame(camera);

	if(m_ActiveCubemap)
	{ 
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);

		Ref<Shader> SkyboxShader = GetPipeline()->GetRenderAPI()->GetShaderFromCache("SkyBoxShader");
		SkyboxShader->Bind();
		SkyboxShader->SetUniform1i("u_CubeMap", 0);
		SkyboxShader->SetUniformMat4f("u_View", camera->GetViewMatrix());
		SkyboxShader->SetUniformMat4f("u_Projection", camera->GetProjectionMatrix());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_ActiveCubemap->m_CubeMapSlot);

		RenderUtils::RenderCube();

		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
	}
}

void CubemapRenderer::ClearFrame()
{

}

Ref<Cubemap> CubemapRenderer::CreateCubemapFromAsset(Ref<TextureAsset> asset)
{
	if (!GetPipeline()->GetRenderAPI()->IsShaderInCache("BasicShader"))
		GetPipeline()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\BasicShader.shader"), "BasicShader");
	if (!GetPipeline()->GetRenderAPI()->IsShaderInCache("CubemapProjector"))
		GetPipeline()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\CubeMapProjector.shader"), "CubemapProjector");
	if (!GetPipeline()->GetRenderAPI()->IsShaderInCache("IrradianceShader"))
		GetPipeline()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\IrradianceShader.shader"), "IrradianceShader");
	if (!GetPipeline()->GetRenderAPI()->IsShaderInCache("PreFilterShader"))
		GetPipeline()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\PreFilterShader.shader"), "PreFilterShader");
	if (!GetPipeline()->GetRenderAPI()->IsShaderInCache("BrdfShader"))
		GetPipeline()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\BrdfShader.shader"), "BrdfShader");
	if (!GetPipeline()->GetRenderAPI()->IsShaderInCache("SkyBoxShader"))
		GetPipeline()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\SkyBoxShader.shader"), "SkyBoxShader");

	Ref<Cubemap> outCubemap = MakeRef<Cubemap>();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL);

	//load the hdr map
	float* data = (float*)asset->m_Pixels.data();
	if (asset)
	{
		glGenTextures(1, &outCubemap->m_hdrTextureSlot);
		glBindTexture(GL_TEXTURE_2D, outCubemap->m_hdrTextureSlot);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, asset->m_width, asset->m_height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		//fail
		ASSERT(false);
	}

	//make cubemap framebuffer
	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	// pbr: setup cubemap to render to and attach to framebuffer
	glGenTextures(1, &outCubemap->m_CubeMapSlot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, outCubemap->m_CubeMapSlot);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//views
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	Ref<Shader> CubeMapProjector = GetPipeline()->GetRenderAPI()->GetShaderFromCache("CubemapProjector");
	CubeMapProjector->Bind();
	CubeMapProjector->SetUniform1i("u_CubeMap", 0);
	CubeMapProjector->SetUniformMat4f("u_Projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, outCubemap->m_hdrTextureSlot);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (uint i = 0; i < 6; ++i)
	{
		CubeMapProjector->SetUniformMat4f("u_View", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, outCubemap->m_CubeMapSlot, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderUtils::RenderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, outCubemap->m_CubeMapSlot);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glGenTextures(1, &outCubemap->m_IrradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, outCubemap->m_IrradianceMap);
	for (uint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	Ref<Shader> IrradianceShader = GetPipeline()->GetRenderAPI()->GetShaderFromCache("IrradianceShader");
	IrradianceShader->Bind();
	IrradianceShader->SetUniform1i("u_CubeMap", 0);
	IrradianceShader->SetUniformMat4f("u_Projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, outCubemap->m_CubeMapSlot);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		IrradianceShader->SetUniformMat4f("u_View", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, outCubemap->m_IrradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderUtils::RenderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//crate pre filter map
	glGenTextures(1, &outCubemap->m_PreFilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, outCubemap->m_PreFilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	Ref<Shader> PreFilterShader = GetPipeline()->GetRenderAPI()->GetShaderFromCache("PreFilterShader");
	PreFilterShader->Bind();
	PreFilterShader->SetUniform1i("u_CubeMap", 0);
	PreFilterShader->SetUniformMat4f("u_Projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, outCubemap->m_CubeMapSlot);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		PreFilterShader->SetUniform1f("u_Roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			PreFilterShader->SetUniformMat4f("u_View", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, outCubemap->m_PreFilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderUtils::RenderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &outCubemap->m_brdfMap);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, outCubemap->m_brdfMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 512, 512, 0, GL_RGBA, GL_FLOAT, 0);
	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outCubemap->m_brdfMap, 0);

	Ref<Shader> BrdfShader = GetPipeline()->GetRenderAPI()->GetShaderFromCache("BrdfShader");
	glViewport(0, 0, 512, 512);
	BrdfShader->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderUtils::RenderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDepthFunc(GL_LESS);

	//delete stuff
	glDeleteFramebuffers(1, &captureFBO);
	glDeleteRenderbuffers(1, &captureRBO);

	return outCubemap;
}

void CubemapRenderer::SetActiveCubemap(Ref<Cubemap> ActiveCubemap)
{
	m_ActiveCubemap = ActiveCubemap;
}

void CubemapRenderer::ClearActiveCubemap()
{
	m_ActiveCubemap = nullptr;
}

