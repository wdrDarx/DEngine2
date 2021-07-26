#include "FrameBuffer.h"
#include "RenderAPI.h"

FrameBuffer::FrameBuffer(const FrameBuferSpec& spec) : m_Spec(spec)
{
	Invalidate();
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_RendererID);
	glDeleteTextures(1, &m_ColorAttachement);
	glDeleteTextures(1, &m_DepthAttachement);
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glViewport(0,0, m_Spec.Width, m_Spec.Height);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Invalidate()
{
	if (m_RendererID != 0)
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachement);
		glDeleteTextures(1, &m_DepthAttachement);
	}

	glCreateFramebuffers(1, &m_RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachement);
	glBindTexture(GL_TEXTURE_2D, m_ColorAttachement);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Spec.Width, m_Spec.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachement);
	glBindTexture(GL_TEXTURE_2D, m_DepthAttachement);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Spec.Width, m_Spec.Height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachement, 0);

	ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);	
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachement, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Resize(const uint& Width, const uint& Height)
{
	m_Spec.Width = Width;
	m_Spec.Height = Height;
	
	glViewport(0, 0, m_Spec.Width, m_Spec.Height);
	Invalidate();
}
