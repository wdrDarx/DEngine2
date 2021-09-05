#include "FrameBuffer.h"


FrameBuffer::FrameBuffer(const FrameBufferSpec& spec) : m_Spec(spec)
{
	Invalidate();
}

FrameBuffer::~FrameBuffer()
{
	if (m_MainFrameBuffer != 0)
	{
		glDeleteFramebuffers(1, &m_MainFrameBuffer);

		if (m_MainColorAttachement > 0)
			glDeleteTextures(1, &m_MainColorAttachement);

		if (m_MainDepthRenderBuffer > 0)
			glDeleteRenderbuffers(1, &m_MainDepthRenderBuffer);
	}

	if (m_IntermidiateBuffer != 0)
	{
		glDeleteFramebuffers(1, &m_IntermidiateBuffer);

		if (m_IntermidiateColorAttachment > 0)
			glDeleteTextures(1, &m_IntermidiateColorAttachment);

		if (m_IntermidiateDepthAttachment > 0)
			glDeleteTextures(1, &m_IntermidiateDepthAttachment);
	}
}

uint FrameBuffer::GetColorAttachement() const
{
	if (m_Spec.Samples <= 1)
	{
		return m_MainColorAttachement;
	}
	else
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MainFrameBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_IntermidiateBuffer);
		glBlitFramebuffer(0, 0, m_Spec.Width, m_Spec.Height, 0, 0, m_Spec.Width, m_Spec.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return m_IntermidiateColorAttachment;
	}
}

uint FrameBuffer::GetDepthAttachement() const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MainFrameBuffer);
	glBindRenderbuffer(GL_DRAW_FRAMEBUFFER, m_IntermidiateBuffer);
	glBlitFramebuffer(0, 0, m_Spec.Width, m_Spec.Height, 0, 0, m_Spec.Width, m_Spec.Height, GL_DEPTH_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return m_IntermidiateDepthAttachment;
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_MainFrameBuffer);
	glViewport(0,0, m_Spec.Width, m_Spec.Height);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Invalidate()
{
	GLuint TextureType;
	TextureType = m_Spec.Samples <= 1 ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;

	//gen main framebuffer
	{
		if (m_MainFrameBuffer != 0)
		{
			glDeleteFramebuffers(1, &m_MainFrameBuffer);

			if(m_MainColorAttachement > 0)
				glDeleteTextures(1, &m_MainColorAttachement);

			if (m_MainDepthRenderBuffer > 0)
				glDeleteRenderbuffers(1, &m_MainDepthRenderBuffer);
		}

		glCreateFramebuffers(1, &m_MainFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_MainFrameBuffer);

		if(m_Spec.CreateColorTexture)
		{ 
			glCreateTextures(TextureType, 1, &m_MainColorAttachement);
			glBindTexture(TextureType, m_MainColorAttachement);
			if (TextureType == GL_TEXTURE_2D)
			{
				glTexImage2D(TextureType, 0, m_Spec.ColorFormat, m_Spec.Width, m_Spec.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples, m_Spec.ColorFormat, m_Spec.Width, m_Spec.Height, GL_TRUE);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureType, m_MainColorAttachement, 0);
		}

		if(m_Spec.CreateDepthTexture)
		{ 
			glGenRenderbuffers(1, &m_MainDepthRenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, m_MainDepthRenderBuffer);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Spec.Samples, m_Spec.DepthFormat, m_Spec.Width, m_Spec.Height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0); //unbind the render buffer
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_MainDepthRenderBuffer);
		}
		
		ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//generate intermediate if the frame buffer is multisampled
	if (TextureType == GL_TEXTURE_2D_MULTISAMPLE)
	{
		if (m_IntermidiateBuffer != 0)
		{
			glDeleteFramebuffers(1, &m_IntermidiateBuffer);

			if(m_IntermidiateColorAttachment > 0)
				glDeleteTextures(1, &m_IntermidiateColorAttachment);

			if (m_IntermidiateDepthAttachment > 0)
				glDeleteTextures(1, &m_IntermidiateDepthAttachment);
		}

		glCreateFramebuffers(1, &m_IntermidiateBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_IntermidiateBuffer);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_IntermidiateColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_IntermidiateColorAttachment);

		glTexImage2D(GL_TEXTURE_2D, 0, m_Spec.ColorFormat, m_Spec.Width, m_Spec.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_IntermidiateColorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_IntermidiateDepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_IntermidiateDepthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, m_Spec.DepthFormat, m_Spec.Width, m_Spec.Height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_IntermidiateDepthAttachment, 0);

		ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void FrameBuffer::Resize(const uint& Width, const uint& Height)
{
	m_Spec.Width = Width;
	m_Spec.Height = Height;
	
	glViewport(0, 0, m_Spec.Width, m_Spec.Height);
	Invalidate();
}
