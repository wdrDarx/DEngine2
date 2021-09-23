#include "DefaultPipeline.h"
#include "DEngine.h"

void DefaultPipeline::OnConstruct()
{
	Super::OnConstruct();

	if (!GetRenderAPI()->IsShaderInCache("DefaultPostProcess"))
	{
		GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "\\Shaders\\" + "DefaultPostProcess.shader"), "DefaultPostProcess");
	}

	
	m_AntiAliasingQuality = 5;

	m_BaseFrameSpec.Height = 1920;
	m_BaseFrameSpec.Width = 1080;
	m_BaseFrameSpec.ColorFormat = GL_RGB32F;
	m_BaseFrameSpec.Samples = 1;
	switch (m_AntiAliasingQuality)
	{
		default :
		{		
			break;
		}

		case 2 : 
		{
			m_BaseFrameSpec.Samples = 2;
			break;
		}

		case 3:
		{
			m_BaseFrameSpec.Samples = 4;
			break;
		}

		case 4:
		{
			m_BaseFrameSpec.Samples = 8;
			break;
		}

		case 5:
		{
			m_BaseFrameSpec.Samples = 16;
			break;
		}
	}


	CreateRenderer<CubemapRenderer>(ObjectInitializer());
	CreateRenderer<QuadRenderer>(ObjectInitializer());
	CreateRenderer<MeshRenderer>(ObjectInitializer());
	CreateRenderer<DebugRenderer>(ObjectInitializer());
}

void DefaultPipeline::PrepareFrame()
{
	Super::PrepareFrame();

	if (m_CurrentRenderTargetIndex + 1 < m_LastNumberOfRenderTargets)
	{
		m_BaseFrames.resize(m_CurrentRenderTargetIndex + 1);
	}

	m_LastNumberOfRenderTargets = m_CurrentRenderTargetIndex + 1;

	//reset index
	m_CurrentRenderTargetIndex = 0; 
}

void DefaultPipeline::RenderFrame(Ref<Camera> camera)
{
	if(true) //post processing
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

		Ref<FrameBuffer> BaseFrame = GetRenderTargetFromIndex(m_CurrentRenderTargetIndex);
		m_CurrentRenderTargetIndex++; //increase for the next render target

		BaseFrame->Bind();
		BaseFrame->Resize(CurrentViewport.x * m_RenderScale, CurrentViewport.y * m_RenderScale);
		GetRenderAPI()->Clear();
		GetRenderAPI()->SetViewport(CurrentViewport * m_RenderScale);
		Super::RenderFrame(camera);
		BaseFrame->Unbind();

		Ref<Shader> PostProcessShader = GetRenderAPI()->GetShaderFromCache("DefaultPostProcess");
		PostProcessShader->Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, BaseFrame->GetColorAttachement());
		PostProcessShader->SetUniform1i("u_BaseFrame", 0);
		PostProcessShader->SetUniform1i("u_UseFXAA", m_AntiAliasingQuality == 1);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, DrawFrameBuffer);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, ReadFrameBuffer);
		GetRenderAPI()->SetViewport(CurrentViewport);
		RenderUtils::RenderQuad();
	}
	else
		Super::RenderFrame(camera);
}

Ref<FrameBuffer> DefaultPipeline::GetRenderTargetFromIndex(uint index)
{
	if (index < m_BaseFrames.size())
	{
		return m_BaseFrames[index];
	}	
	else
	{
		m_BaseFrames.push_back(MakeRef<FrameBuffer>(m_BaseFrameSpec));

		return GetRenderTargetFromIndex(index);
	}
}

