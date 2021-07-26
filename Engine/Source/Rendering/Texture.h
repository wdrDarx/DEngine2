#pragma once
#include "RenderAPI.h"

struct DENGINE_API Texture
{
private:
	byte* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
public:
	uint m_RendererID;
	Texture(const std::string& path);

	Texture();
	Texture(const uint& width, const uint& height, unsigned char* pixeldata);
	
	~Texture();

	bool operator==(const Texture& other) const
	{
		return m_RendererID == other.m_RendererID;
	}

	void Bind(uint slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; };
	inline int GetHeight() const { return m_Height; };


};

