#pragma once
#include "RenderAPI.h"
#include "Utils/ImageLoader.h"

struct DENGINE_API Texture
{
private:
	int m_Width, m_Height, m_BPP;
public:
	uint m_RendererID;
	Texture(const Image& image);
	Texture(const TextureSpec& spec);
	Texture(const TextureSpec& spec, const uint& width, const uint& height, byte* pixeldata);
	~Texture();

	bool operator==(const Texture& other) const
	{
		return m_RendererID == other.m_RendererID;
	}

	void Bind(uint slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; };
	inline int GetHeight() const { return m_Height; };

private:
	void Construct(const TextureSpec& spec, const uint& width, const uint& height, byte* pixeldata);
};

