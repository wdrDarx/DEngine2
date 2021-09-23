#pragma once
#include "Core/Core.h"
#include "RenderAPI.h"

struct DENGINE_API FrameBufferSpec
{
	uint Width, Height;
	uint Samples = 1;
	uint ColorFormat = GL_RGBA8;
	uint DepthFormat = GL_DEPTH32F_STENCIL8;
	uint IntermediateColorFormat = GL_RGBA8;
	uint IntermediateDepthFormat = GL_DEPTH32F_STENCIL8;
	bool CreateDepthTexture = true;
	bool CreateColorTexture = true;
};

struct DENGINE_API FrameBuffer
{
public:
	FrameBuffer(const FrameBufferSpec& spec);
	~FrameBuffer();
	const FrameBufferSpec& GetSpec() const {return m_Spec; };
	
	uint GetColorAttachement() const;
	uint GetDepthAttachement() const;

	void Bind();
	void Unbind();
	void Invalidate();

	void Resize(const uint& Width, const uint& Height);

private:
	uint m_MainFrameBuffer = 0;
	uint m_IntermidiateBuffer = 0;

	uint m_MainColorAttachement = 0;
	uint m_MainDepthRenderBuffer = 0;

	uint m_IntermidiateColorAttachment = 0;
	uint m_IntermidiateDepthAttachment = 0;
	FrameBufferSpec m_Spec;
};

