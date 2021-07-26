#pragma once
#include "Core/Core.h"

struct DENGINE_API FrameBuferSpec
{
	uint Width, Height;
	uint Samples = 1;
	bool SwapChainTarget = false;
};

struct DENGINE_API FrameBuffer
{
public:
	FrameBuffer(const FrameBuferSpec& spec);
	~FrameBuffer();
	const FrameBuferSpec& GetSpec() const {return m_Spec; };
	
	uint GetColorAttachement() const {return m_ColorAttachement;};
	uint GetDepthAttachement() const {return m_DepthAttachement;};

	void Bind();
	void Unbind();
	void Invalidate();

	void Resize(const uint& Width, const uint& Height);

private:
	uint m_RendererID;
	uint m_ColorAttachement;
	uint m_DepthAttachement;
	FrameBuferSpec m_Spec;
};

