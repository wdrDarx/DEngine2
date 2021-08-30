#pragma once
#include "Core/Core.h"

struct DENGINE_API FrameBufferSpec
{
	uint Width, Height;
	uint Samples = 1;
	bool SwapChainTarget = false;
};

struct DENGINE_API FrameBuffer
{
public:
	FrameBuffer(const FrameBufferSpec& spec);
	~FrameBuffer();
	const FrameBufferSpec& GetSpec() const {return m_Spec; };
	
	uint GetColorAttachement() const {return m_ColorAttachement;};
	uint GetDepthAttachement() const {return m_DepthAttachement;};

	void Bind();
	void Unbind();
	void Invalidate();

	void Resize(const uint& Width, const uint& Height);

private:
	uint m_RendererID = 0;
	uint m_ColorAttachement = 0;
	uint m_DepthAttachement = 0;
	FrameBufferSpec m_Spec;
};

